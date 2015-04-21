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
 * Authors: Rene Glebke, Martin Lang
 * Contributors: Taejin Park
 */

#ifndef REQUESTSCHEDULINGSTRATEGYBASE_H_
#define REQUESTSCHEDULINGSTRATEGYBASE_H_

#include "AbstractStrategy.h"

#include "PushPullPeer.h"

namespace ns3 {
namespace pushpull {

class Peer;

/**
 * \ingroup PushPull
 *
 * \brief Provides base functionality for handling incoming requests from other PushPull clients.
 *
 * This class provides a basic implementation for the handling of upload requests (mainly REQUEST messages [PushPull message id 6]) received by a client.
 * You may override or add handler functions for events generated by the PushPullClient class and derivated classes to implement other
 * or additional behavior, such as upload traffic shaping.
 *
 * The base implementation checks whether the requesting client is currently unchoked. If so, it directly initiates a transfer of the piece
 * to the requester; else, it silently drops the request.
 */
class RequestSchedulingStrategyBase : public AbstractStrategy
{
// Constructors etc.
public:
  RequestSchedulingStrategyBase (Ptr<PushPullClient> myClient);
  virtual ~RequestSchedulingStrategyBase ();
  /**
   * Initialize the strategy. Register the needed event listeners with the associated client.
   */
  virtual void DoInitialize ();

// Event handlers
public:
  /**
   * \brief Event handler for received REQUEST messages. Implements the main functionality.
   *
   * @param peer Pointer to the Peer object associated with the client that sent the request.
   * @param pieceIndex the index of the requested piece.
   * @param blockOffset the offset of the requested piece.
   * @param blockLength the length of the requested block.
   */
  virtual void ProcessPeerRequestEvent (Ptr<Peer> peer, uint32_t pieceIndex, uint32_t blockOffset, uint32_t blockLength);
};

} // ns pushpull
} // ns ns3

#endif /* REQUESTSCHEDULERBASE_H_ */
