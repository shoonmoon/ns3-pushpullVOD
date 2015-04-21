/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010-2013 ComSys, RWTH Aachen University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Martin Lang, Elias Weingaertner (principal & initial authors), Rene Glebke, Alexander Hocks
 */

#include "MediaData.h"

#include "ns3/log.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <ios>

namespace ns3 {
namespace pushpull {

NS_LOG_COMPONENT_DEFINE ("pushpull::MediaData");

MediaData::MediaData ()
{

}

MediaData::~MediaData ()
{
}

TypeId MediaData::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::pushpull::MediaData").SetParent<Object> ();

  return tid;
}

bool MediaData::ReadMediaDataFile (std::string path)
{
  // First, we read the torrent file
  std::ifstream torrentFile (path.c_str (),std::ios_base::in | std::ios_base::binary);

  if (!torrentFile.is_open ())
    {
      NS_LOG_ERROR ("Error: Could not open torrent file \"" << path << "\".");
      return false;
    }

  std::string wholeMediaDataFile;
  torrentFile.seekg (0, std::ios::end);
  wholeMediaDataFile.reserve (static_cast<uint32_t> (torrentFile.tellg ()));
  torrentFile.seekg (0, std::ios::beg);
  wholeMediaDataFile.assign ((std::istreambuf_iterator<char> (torrentFile)), std::istreambuf_iterator<char> ());

  size_t infoPos = wholeMediaDataFile.find (":infod") + 5;
  size_t eeePos = wholeMediaDataFile.find ("eee");
  
  // In case the torrent file finishes with "ee" instead of "eee"
  if (eeePos == std::string::npos)
    {
      eeePos = wholeMediaDataFile.rfind ("ee");
      if (eeePos == std::string::npos)
        {
          NS_LOG_ERROR ("Error: Could not find the 'ee' sequence near the end of \"" << path << "\". Aborting.");
        }
      else
        eeePos += 1;
    }
  else
    {
      eeePos += 2;
    }

  std::string infoValue = wholeMediaDataFile.substr (infoPos, eeePos - infoPos);

  // Next, we calculate the SHA1 hash over the content of the torrent file
  unsigned char newSHA[20];
  char SHAhexstring[41];
  sha1::calc(&infoValue[0], eeePos - infoPos, newSHA);
  sha1::toHexString(newSHA, SHAhexstring);

  std::stringstream ss1;
  ss1 << std::uppercase << SHAhexstring;
  std::string sha1Hash = ss1.str();
  std::transform(sha1Hash.begin(), sha1Hash.end(), sha1Hash.begin(), ::toupper);
  m_infoHash = sha1Hash;
  memcpy (m_byteValueInfoHash, newSHA, 20);

  // Output has byte value string
  std::memcpy (m_byteValueInfoHash, newSHA, 20);

  // Output as hex string
  std::stringstream ss;
  ss << SHAhexstring;
  m_infoHash = ss.str ();

  // Output as binary URL encoded hex
  std::stringstream encHashStream;
  for (uint32_t i = 0; i < 20; ++i)
    {
      if ((newSHA[i] <= 44) || (newSHA[i] == 47) || (newSHA[i] >= 58 && newSHA[i] <= 64) || (newSHA[i] >= 91 && newSHA[i] <= 96) || (newSHA[i] >= 123 && newSHA[i] != 126))
        {
          encHashStream
          << "%"
          << std::uppercase << std::right << std::setw (2) << std::setfill ('0') << std::hex
          << static_cast<uint16_t> (static_cast<uint8_t> (newSHA[i]));
        }
      else if ((newSHA[i] >= 97 && newSHA[i] <= 127) || (newSHA[i] >= 48 && newSHA[i] <= 57) || (newSHA[i] >= 65 && newSHA[i] <= 90))
        {
          std::string strConverter = "";
          strConverter += newSHA[i];
          encHashStream << strConverter;
        }
      else
        {
          encHashStream << newSHA[i];
        }
    }
  m_encodedInfoHash = encHashStream.str ();

  // Now, read and apply the content of the torrent file
  torrentFile.seekg (0, std::ios::beg); // return get pointer to start of inputstream
  Ptr<MediaDataData> root = MediaDataData::ReadBencodedData (torrentFile);
  Ptr<MediaDataDataDictonary> rootDict = DynamicCast<MediaDataDataDictonary> (root);

  NS_ASSERT (rootDict);

  rootDict->Dump ();

  // now copy the mandatory attributes in our data structure

  Ptr<MediaDataDataString> annURL = DynamicCast<MediaDataDataString> (rootDict->GetData ("announce"));
  NS_ASSERT (annURL);
  m_announceURL = annURL->GetData ();

  Ptr<MediaDataDataDictonary> infoDict = DynamicCast<MediaDataDataDictonary> (rootDict->GetData ("info"));
  NS_ASSERT (infoDict);

  Ptr<MediaDataDataString> torrentName = DynamicCast<MediaDataDataString> (infoDict->GetData ("name"));
  NS_ASSERT (torrentName);

  m_fileName = torrentName->GetData ();

  Ptr<MediaDataDataInt> torrentfileSize = DynamicCast<MediaDataDataInt> (infoDict->GetData ("length"));
  NS_ASSERT (torrentfileSize);

  m_fileLength = torrentfileSize->GetData ();

  Ptr<MediaDataDataInt> torrentPieceSize = DynamicCast<MediaDataDataInt> (infoDict->GetData ("piece length"));
  NS_ASSERT (torrentPieceSize);

  m_pieceLength = torrentPieceSize->GetData ();

  // calculate the number of pieces
  m_numberOfPieces = static_cast<uint32_t> (m_fileLength / m_pieceLength);

  // if it does not fit exactly we have a trailing piece
  m_trailingPieceLength = m_fileLength % m_pieceLength;
  if (m_trailingPieceLength > 0)
    {
      ++m_numberOfPieces;
    }

  // Set the bitfield size so we can easily travere the bitfield in applications without having to store this value for each app instance
  m_bitfieldSize = m_numberOfPieces / 8;
  if ((m_numberOfPieces % 8) > 0)
    {
      ++m_bitfieldSize;
    }

  // now read all the hashes and store them
  Ptr<MediaDataDataString> hashes = DynamicCast<MediaDataDataString> (infoDict->GetData ("pieces"));
  NS_ASSERT (hashes);

  NS_ASSERT (hashes->GetData ().size () == 20 * m_numberOfPieces);

  const char *data = hashes->GetData ().data ();

  m_pieces.reserve (m_numberOfPieces);

  for (unsigned int i = 0; i < m_numberOfPieces; ++i)
    {
      std::memcpy (m_pieces[i].sha_hash,data,20);
      data += 20;
    }

  // now we read the optional parameters
  Ptr<MediaDataDataInt> cdate = DynamicCast<MediaDataDataInt> (rootDict->GetData ("creation date"));
  if (cdate)
    {
      m_creationDate = static_cast<time_t> (cdate->GetData ());
    }

  Ptr<MediaDataDataString> torrentComment = DynamicCast<MediaDataDataString> (rootDict->GetData ("comment"));
  if (torrentComment)
    {
      m_comment = torrentComment->GetData ();
    }
  else
    {
      Ptr<MediaDataDataString> torrentComment2 = DynamicCast<MediaDataDataString> (rootDict->GetData ("comment.utf-8"));
      if (torrentComment2)
        {
          m_comment = torrentComment2->GetData ();
        }
    }

  Ptr<MediaDataDataString> torrentEnconding = DynamicCast<MediaDataDataString> (rootDict->GetData ("encoding"));
  if (torrentEnconding)
    {
      m_encoding = torrentEnconding->GetData ();
    }
  else
    {
      m_encoding = "utf8";           // this is standard
    }

  return true;
}

void MediaData::SetDataPath (std::string dataPath)
{
  m_dataPath = dataPath;
}

std::string MediaData::GetDataPath () const
{
  return m_dataPath;
}

std::string MediaData::GetAnnounceURL () const
{
  return m_announceURL;
}

void MediaData::SetAnnounceURL (std::string announceURL)
{
  m_announceURL = announceURL;
}

std::string MediaData::GetInfoHash () const
{
  return m_infoHash;
}
std::string MediaData::GetEncodedInfoHash () const
{
  return m_encodedInfoHash;
}
const char* MediaData::GetByteValueInfoHash () const
{
  return (const char*) &m_byteValueInfoHash[0];
}

time_t MediaData::GetCreationDate () const
{
  return m_creationDate;
}

std::string MediaData::GetComment () const
{
  return m_comment;
}

std::string MediaData::GetEncoding () const
{
  return m_encoding;
}

const char* MediaData::GetPieces () const
{
  return (const char*) &m_pieces[0].sha_hash;
}

uint8_t MediaData::IsPrivateMediaData () const
{
  return m_privateMediaData;
}

uint8_t MediaData::GetFileMode () const
{
  return m_fileMode;
}

std::string  MediaData::GetFileName () const
{
  return m_fileName;
}

uint64_t MediaData::GetFileLength () const
{
  return m_fileLength;
}

bool MediaData::HasTrailingPiece () const
{
  return m_trailingPieceLength > 0;
}

uint8_t MediaData::GetNumberOfFiles () const
{
  return m_numberOfiles;
}

uint32_t MediaData::GetBitfieldSize () const
{
  return m_bitfieldSize;
}

uint32_t MediaData::GetTrailingPieceLength () const
{
  return m_trailingPieceLength;
}

} // ns pushpull
} // ns ns3
