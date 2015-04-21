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

#ifndef PROTOCOLFACTORY_H_
#define PROTOCOLFACTORY_H_

#include "AbstractStrategy.h"

#include "PushPullClient.h"
#include "PeerConnectorStrategyBase.h"

namespace ns3 {
namespace pushpull {

/**
 * \ingroup PushPull
 *
 * \brief A factory for PushPull-based protocol implementations.
 *
 * This factory can be used to create and assign the strategy instances necessary to implement a specific PushPull-based protocol
 * on one instance of the PushPullClient class or its derivatives.
 *
 * You can implement additional member functions to create sets of strategies (referred to as "bundles") for further protocols.
 */
class ProtocolFactory : public Object
{
public:
// Constructors etc.
  ProtocolFactory ();
  virtual ~ProtocolFactory ();

// Main interaction method
public:
  /**
   * \brief Create and initialize a bundle of strategy instances that together implement a specific PushPull-based protocol and attach them to one PushPullClient class instance.
   *
   * After a call to this method, instances of the strategy classes constituting the chosen protocol are associated with the client.
   *
   * Available protocol (protocolName argument, see below) in the default implementation are:
   *
   * * (empty) Default PushPull protocol according to the description on <a href="http://wiki.theory.org/PushPullSpecification" target="_blank">theory.org</a> with a sequential piece selection mechanism.
   *
   * * "rarest-first" Default PushPull protocol according to the description on <a href="http://wiki.theory.org/PushPullSpecification" target="_blank">theory.org</a> with a rarest-first selection mechanism.
   *
   * Note: Strategy implementations usually require the network of the client and the internal bitfield of the client to be readily initialized.
   * You should not call this method before this state has been reached.
   *
   * @param protocolName the name (= internal identifier) of the strategy bundle (= protocol) to create.
   * @param client the client that the strategy bundle is created for. Each client client instance should have exactly one strategy bundle assigned to it.
   * @param strategyStore address of a data structure in the client storing the pointers to the created strategy instances for later reference.
   * @param peerConnectorStrategy address of the data structure in the client storing the pointer to the part of the strategy bundle responsible for connecting peers.
   */
  static void                     CreateStrategyBundle (std::string protocolName, Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& peerConnectorStrategy);

// Methods for creating the implemented protocols
private:
  // Creates the standard PushPull protocol with sequential piece selection (i.e., from "left to right" through the bitfield)
  static void                     CreateBasicProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& peerConnectorStrategy);
  // Creates the standard PushPull protocol with the rarest-first piece selection heuristic
  static void                     CreateRarestFirstProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& peerConnectorStrategy);

  // RENE: NOT YET PORTED TO NEW VERSION: Creates the standard PushPull protocol with rarest-first heuristic that leaves out pieces before the playback point
  static void                     CreateRarestFirstVoDProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& peerConnectorStrategy);

  // RENE: NOT YET PORTED TO NEW VERSION: Creates the BiToS protocol by Vlavianos, Iliofotou and Faloutsos
  static void                     CreateBiToSProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& peerConnectorStrategy);
  // RENE: NOT YET PROTED TO NEW VERSION: Creates the Give-to-Get protocol by Mol, Pouwelse, Meulpolder, Epema and Sips
  static void                     CreateGiveToGetProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& peerConnectorStrategy);
};

} // ns pushpull
} // ns ns3
#endif /* PROTOCOLFACTORY_H_ */
