/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010-2011 ComSys, RWTH Aachen University
 * Partially copyright (c) 2014-2015 Yonsei University
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
 * Authors: Martin Lang (principal author), Rene Glebke
 * Contributors: Taejin Park
 */

#include "PushPullPacket.h"

#include "ns3/log.h"
#include "ns3/packet.h"

#include <cstring>

namespace ns3 {
namespace pushpull {

/************************************************************************************************/
/************************************** PushPullHandshakeMessage *************************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullHandshakeMessage);

PushPullHandshakeMessage::PushPullHandshakeMessage ()
{
}

PushPullHandshakeMessage::~PushPullHandshakeMessage ()
{
}

void PushPullHandshakeMessage::Serialize (Buffer::Iterator start) const
{
  NS_ASSERT (m_protocol.size () <= 256);
  uint8_t buffer[8];
  std::memset (buffer,0,8);

  start.WriteU8 ((uint8_t)m_protocol.size ());
  start.Write (reinterpret_cast<const uint8_t*> (m_protocol.c_str ()),m_protocol.size ());
  // The next few bits are reserved space
  start.Write (buffer, 5);
  // Announce that we support the extension protocol
  uint8_t extensionBit = 0x10;
  start.WriteU8 (extensionBit);
  start.Write (buffer, 2);
  // Write the rest of the message
  start.Write (m_infoHash,20);
  start.Write (m_peerId,20);
}

uint32_t PushPullHandshakeMessage::Deserialize (Buffer::Iterator start)
{
  // Two-step reading: First the length of the protocol string, then the protocol string itself
  uint8_t buffer[257];

  unsigned char pstrLen = start.ReadU8 ();
  start.Read (buffer,pstrLen);      // protocol string
  m_protocol = reinterpret_cast<char*> (buffer);
  start.Read (buffer,8);      // Reserved space; TODO: Read out announcements for "extension protocol" messages (see Serialize())
  start.Read (m_infoHash,20);
  start.Read (m_peerId,20);
  return pstrLen + 49;
}

TypeId PushPullHandshakeMessage::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullHandshakeMessage").SetParent<Header> ()
    .AddConstructor<PushPullHandshakeMessage> ();

  return tid;
}

/************************************************************************************************/
/************************************** PushPullLengthHeader **********************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullLengthHeader);

PushPullLengthHeader::PushPullLengthHeader ()
{
  m_len = 0;
}

PushPullLengthHeader::PushPullLengthHeader (uint32_t len)
{
  m_len = len;
}

PushPullLengthHeader::~PushPullLengthHeader ()
{

}

uint32_t PushPullLengthHeader::Deserialize (Buffer::Iterator start)
{
  m_len = start.ReadNtohU32 ();

  return 4;
}

void PushPullLengthHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteHtonU32 (m_len);
}

TypeId PushPullLengthHeader::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullLengthHeader").SetParent<Header> ()
    .AddConstructor<PushPullLengthHeader> ();

  return tid;
}

/************************************************************************************************/
/************************************** PushPullTypeHeader ************************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullTypeHeader);

PushPullTypeHeader::PushPullTypeHeader ()
{
}

PushPullTypeHeader::PushPullTypeHeader (PushPullMessageType type)
{
  m_type = type;
}

PushPullTypeHeader::~PushPullTypeHeader ()
{
}

uint32_t PushPullTypeHeader::Deserialize (Buffer::Iterator start)
{
  m_type = static_cast<PushPullMessageType> (start.ReadU8 ());

  return 1;

}

void PushPullTypeHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU8 (static_cast<uint8_t> (m_type));
}

TypeId PushPullTypeHeader::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullTypeHeader").SetParent<Header> ()
    .AddConstructor<PushPullTypeHeader> ();

  return tid;
}

/************************************************************************************************/
/***************************************** PushPullHaveMessage ******************************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullHaveMessage);

PushPullHaveMessage::PushPullHaveMessage ()
{
}

PushPullHaveMessage::PushPullHaveMessage (uint32_t pieceIndex)
{
  m_pieceIndex = pieceIndex;
}

PushPullHaveMessage::~PushPullHaveMessage ()
{
}

void PushPullHaveMessage::Serialize (Buffer::Iterator start) const
{
  start.WriteHtonU32 (m_pieceIndex);
}

uint32_t PushPullHaveMessage::Deserialize (Buffer::Iterator start)
{
  m_pieceIndex = start.ReadNtohU32 ();
  return 4;
}

TypeId PushPullHaveMessage::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullHaveMessage").SetParent<Header> ()
    .AddConstructor<PushPullHaveMessage> ();

  return tid;
}

/************************************************************************************************/
/************************************** PushPullBitfieldMessage *****************************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullBitfieldMessage);

PushPullBitfieldMessage::PushPullBitfieldMessage ()
{
  m_bitField = 0;
  m_bitFieldSize = 0;
}

PushPullBitfieldMessage::PushPullBitfieldMessage (uint32_t bitFieldSize)
{
  m_bitField = new uint8_t[bitFieldSize];
  m_bitFieldSize = bitFieldSize;
}

PushPullBitfieldMessage::~PushPullBitfieldMessage ()
{
  delete [] m_bitField;
  m_bitField = 0;
}

void PushPullBitfieldMessage::SetBitFieldSize (uint32_t bitFieldSize)
{
  delete [] m_bitField;
  m_bitField = new uint8_t[bitFieldSize];
  m_bitFieldSize = bitFieldSize;
}

void PushPullBitfieldMessage::CopyBitFieldFrom (const std::vector<uint8_t>* sourceField)
{
  for (uint8_t i = 0; i < m_bitFieldSize; ++i)
    {
      m_bitField[i] = (*sourceField)[i];
    }
}

void PushPullBitfieldMessage::CopyBitFieldTo (std::vector<uint8_t>* targetField) const
{
  for (uint8_t i = 0; i < m_bitFieldSize; ++i)
    {
      (*targetField)[i] = m_bitField[i];
    }
}

void PushPullBitfieldMessage::Serialize (Buffer::Iterator start) const
{
  start.Write (m_bitField, m_bitFieldSize);
}

uint32_t PushPullBitfieldMessage::Deserialize (Buffer::Iterator start)
{
  // NOTE: For this method to function properly, the size of the bitfield must be already known (e.g., from the previous PushPullLengthHeader)
  start.Read (m_bitField, m_bitFieldSize);
  return m_bitFieldSize;
}



TypeId PushPullBitfieldMessage::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullBitfieldMessage").SetParent<Header> ()
    .AddConstructor<PushPullBitfieldMessage> ();

  return tid;
}

/************************************************************************************************/
/************************************** PushPullRequestMessage  *****************************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullRequestMessage);

PushPullRequestMessage::PushPullRequestMessage ()
{
}

PushPullRequestMessage::PushPullRequestMessage (uint32_t pieceIndex, uint32_t blockOffset, uint32_t blockLength)
{
  m_pieceIndex = pieceIndex;
  m_blockOffset = blockOffset;
  m_blockLength = blockLength;
}

PushPullRequestMessage::~PushPullRequestMessage ()
{
}

void PushPullRequestMessage::Serialize (Buffer::Iterator start) const
{
  start.WriteHtonU32 (m_pieceIndex);
  start.WriteHtonU32 (m_blockOffset);
  start.WriteHtonU32 (m_blockLength);
}

uint32_t PushPullRequestMessage::Deserialize (Buffer::Iterator start)
{
  m_pieceIndex = start.ReadNtohU32 ();
  m_blockOffset = start.ReadNtohU32 ();
  m_blockLength = start.ReadNtohU32 ();

  return 12;
}

TypeId PushPullRequestMessage::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullRequestMessage").SetParent<Header> ()
    .AddConstructor<PushPullRequestMessage> ();

  return tid;
}

/************************************************************************************************/
/*************************************** PushPullCancelMessage  *****************************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullCancelMessage);

PushPullCancelMessage::PushPullCancelMessage ()
{
}

PushPullCancelMessage::PushPullCancelMessage (uint32_t pieceIndex, uint32_t blockOffset, uint32_t blockLength)
{
  m_pieceIndex = pieceIndex;
  m_blockOffset = blockOffset;
  m_blockLength = blockLength;
}

PushPullCancelMessage::~PushPullCancelMessage ()
{
}

void PushPullCancelMessage::Serialize (Buffer::Iterator start) const
{
  start.WriteHtonU32 (m_pieceIndex);
  start.WriteHtonU32 (m_blockOffset);
  start.WriteHtonU32 (m_blockLength);
}

uint32_t PushPullCancelMessage::Deserialize (Buffer::Iterator start)
{
  m_pieceIndex = start.ReadNtohU32 ();
  m_blockOffset = start.ReadNtohU32 ();
  m_blockLength = start.ReadNtohU32 ();

  return 12;
}

TypeId PushPullCancelMessage::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullCancelMessage").SetParent<Header> ()
    .AddConstructor<PushPullCancelMessage> ();

  return tid;
}

/************************************************************************************************/
/**************************************** PushPullPieceMessage  *****************************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullPieceMessage);

PushPullPieceMessage::PushPullPieceMessage ()
{
}

PushPullPieceMessage::PushPullPieceMessage (uint32_t index, uint32_t begin)
{
  m_index = index;
  m_begin = begin;
}

PushPullPieceMessage::~PushPullPieceMessage ()
{
}

void PushPullPieceMessage::Serialize (Buffer::Iterator start) const
{
  start.WriteHtonU32 (m_index);
  start.WriteHtonU32 (m_begin);
}

uint32_t PushPullPieceMessage::Deserialize (Buffer::Iterator start)
{
  m_index = start.ReadNtohU32 ();
  m_begin = start.ReadNtohU32 ();
  return 8;       // Note: This of course does not include the size of the payload, which is to be read separately from the stream after reading this message HEADER
}

TypeId PushPullPieceMessage::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullPieceMessage").SetParent<Header> ()
    .AddConstructor<PushPullPieceMessage> ();

  return tid;
}

/************************************************************************************************/
/***************************************** PushPullPortMessage  *****************************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullPortMessage);

PushPullPortMessage::PushPullPortMessage ()
{
}

PushPullPortMessage::PushPullPortMessage (uint16_t listenPort)
{
  m_listenPort = listenPort;
}

PushPullPortMessage::~PushPullPortMessage ()
{
}

void PushPullPortMessage::Serialize (Buffer::Iterator start) const
{
  start.WriteHtonU32 (m_listenPort);
}

uint32_t PushPullPortMessage::Deserialize (Buffer::Iterator start)
{
  m_listenPort = start.ReadNtohU16 ();
  return 2;
}

TypeId PushPullPortMessage::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullPortMessage").SetParent<Header> ()
    .AddConstructor<PushPullPortMessage> ();

  return tid;
}


/************************************************************************************************/
/***************************************** PushPullExtensionMessage  ************************************/
/************************************************************************************************/

NS_OBJECT_ENSURE_REGISTERED (PushPullExtensionMessage);

PushPullExtensionMessage::PushPullExtensionMessage ()
{
  m_content = 0;
  m_packetLength = 1;
}

PushPullExtensionMessage::PushPullExtensionMessage (uint32_t packetLength)
{
  NS_ASSERT (packetLength > 1);
  m_packetLength = packetLength;
  m_content = new uint8_t[packetLength - 1];
}

PushPullExtensionMessage::PushPullExtensionMessage (uint8_t messageId, const std::string& content)
{
  m_messageId = messageId;
  m_packetLength = content.size () + 1;  // + 1 for the messageId
  m_content = new uint8_t[content.size ()];
  content.copy (reinterpret_cast<char* > (m_content), content.size (), 0);
}

PushPullExtensionMessage::~PushPullExtensionMessage ()
{
  delete[] m_content;
}

void PushPullExtensionMessage::SetContent (const std::string& content)
{
  m_packetLength = content.size () + 1;  // + 1 for the messageId
  delete[] m_content;
  m_content = new uint8_t[content.size ()];
  content.copy (reinterpret_cast<char* > (m_content), content.size (), 0);
}

void PushPullExtensionMessage::Serialize (Buffer::Iterator start) const
{
  start.WriteU8 (static_cast<uint8_t> (m_messageId));
  start.Write (&m_content[0], m_packetLength - 1);
}

uint32_t PushPullExtensionMessage::Deserialize (Buffer::Iterator start)
{
  m_messageId = start.ReadU8 ();
  start.Read (m_content, m_packetLength - 1);

  return m_packetLength;
}

TypeId PushPullExtensionMessage::GetTypeId ()
{

  static TypeId tid = TypeId ("ns3::pushpull::PushPullExtensionMessage").SetParent<Header> ()
    .AddConstructor<PushPullExtensionMessage> ();

  return tid;
}

std::string PushPullExtensionMessage::GetContent () const
{
  std::string result;
  result.assign (reinterpret_cast<const char*> (m_content), m_packetLength - 1);

  return result;
}

} // ns pushpull
} // ns ns3
