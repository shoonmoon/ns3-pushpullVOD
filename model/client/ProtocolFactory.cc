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

#include "ProtocolFactory.h"

#include "PartSelectionStrategyBase.h"
#include "PeerConnectorStrategyBase.h"
#include "RequestSchedulingStrategyBase.h"

#include "PushPullVideoMetricsBase.h"

#include "strategies/RarestFirstPartSelectionStrategy.h"

namespace ns3 {
namespace pushpull {

NS_LOG_COMPONENT_DEFINE ("pushpull::ProtocolFactory");
NS_OBJECT_ENSURE_REGISTERED (ProtocolFactory);

ProtocolFactory::ProtocolFactory ()
{
}

ProtocolFactory::~ProtocolFactory ()
{
}

void ProtocolFactory::CreateStrategyBundle (std::string protocolName, Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& aPeerConnectorStrategy)
{
  if (protocolName == "default")
    {
      CreateBasicProtocol (client, strategyStore, aPeerConnectorStrategy);
    }
  else if (protocolName == "rarest-first")
    {
      CreateRarestFirstProtocol (client, strategyStore, aPeerConnectorStrategy);
    }
  else if (protocolName == "rarest-first-vod")
    {
      CreateRarestFirstVoDProtocol (client, strategyStore, aPeerConnectorStrategy);
    }
  else if (protocolName == "bitos")
    {
      CreateBiToSProtocol (client, strategyStore, aPeerConnectorStrategy);
    }
  else if (protocolName == "give-to-get")
    {
      CreateGiveToGetProtocol (client, strategyStore, aPeerConnectorStrategy);
    }
  else
    {
      CreateBasicProtocol (client, strategyStore, aPeerConnectorStrategy);
    }
}

void ProtocolFactory::CreateBasicProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& aPeerConnectorStrategy)
{
  Ptr<PeerConnectorStrategyBase> peerConnectorStrategy = Create<PeerConnectorStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (peerConnectorStrategy);
  peerConnectorStrategy->DoInitialize ();

  Ptr<ChokeUnChokeStrategyBase> chokeUnChokeStrategy = Create<ChokeUnChokeStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (chokeUnChokeStrategy);
  chokeUnChokeStrategy->DoInitialize ();

  Ptr<PartSelectionStrategyBase> partSelectionStrategy = Create<PartSelectionStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (partSelectionStrategy);
  partSelectionStrategy->DoInitialize ();

  Ptr<RequestSchedulingStrategyBase> requestSchedulingStrategy = Create<RequestSchedulingStrategyBase, Ptr<PushPullClient > > (client);
  strategyStore.push_back (requestSchedulingStrategy);
  requestSchedulingStrategy->DoInitialize ();

  aPeerConnectorStrategy = peerConnectorStrategy;
}

void ProtocolFactory::CreateRarestFirstProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& aPeerConnectorStrategy)
{
  Ptr<PeerConnectorStrategyBase> peerConnectorStrategy = Create<PeerConnectorStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (peerConnectorStrategy);
  peerConnectorStrategy->DoInitialize ();

  Ptr<ChokeUnChokeStrategyBase> chokeUnChokeStrategy = Create<ChokeUnChokeStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (chokeUnChokeStrategy);
  chokeUnChokeStrategy->DoInitialize ();

  Ptr<RarestFirstPartSelectionStrategy> partSelectionStrategy = Create<RarestFirstPartSelectionStrategy, Ptr<PushPullClient> > (client);
  strategyStore.push_back (partSelectionStrategy);
  partSelectionStrategy->DoInitialize ();

  Ptr<RequestSchedulingStrategyBase> requestSchedulingStrategy = Create<RequestSchedulingStrategyBase, Ptr<PushPullClient > > (client);
  strategyStore.push_back (requestSchedulingStrategy);
  requestSchedulingStrategy->DoInitialize ();

  aPeerConnectorStrategy = peerConnectorStrategy;
}

void ProtocolFactory::CreateRarestFirstVoDProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& aPeerConnectorStrategy)
{
  Ptr<PeerConnectorStrategyBase> peerConnectorStrategy = Create<PeerConnectorStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (peerConnectorStrategy);
  peerConnectorStrategy->DoInitialize ();

  Ptr<ChokeUnChokeStrategyBase> chokeUnChokeStrategy = Create<ChokeUnChokeStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (chokeUnChokeStrategy);
  chokeUnChokeStrategy->DoInitialize ();

//	Ptr<RarestFirstVoDPartSelectionStrategy> partSelectionStrategy = Create<RarestFirstVoDPartSelectionStrategy, Ptr<PushPullClient> >(client);
//	strategyStore.push_back(partSelectionStrategy);
//	partSelectionStrategy->DoInitialize();

  Ptr<RequestSchedulingStrategyBase> requestSchedulingStrategy = Create<RequestSchedulingStrategyBase, Ptr<PushPullClient > > (client);
  strategyStore.push_back (requestSchedulingStrategy);
  requestSchedulingStrategy->DoInitialize ();

  Ptr<PushPullVideoMetricsBase> videoMetricsBase = Create<PushPullVideoMetricsBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (videoMetricsBase);
  videoMetricsBase->DoInitialize ();

  aPeerConnectorStrategy = peerConnectorStrategy;
}

void ProtocolFactory::CreateBiToSProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& aPeerConnectorStrategy)
{
  Ptr<PeerConnectorStrategyBase> peerConnectorStrategy = Create<PeerConnectorStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (peerConnectorStrategy);
  peerConnectorStrategy->DoInitialize ();

  Ptr<ChokeUnChokeStrategyBase> chokeUnChokeStrategy = Create<ChokeUnChokeStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (chokeUnChokeStrategy);
  chokeUnChokeStrategy->DoInitialize ();

//	Ptr<BiToSPartSelectionStrategy> partSelectionStrategy = Create<BiToSPartSelectionStrategy, Ptr<PushPullClient> >(client);
//	strategyStore.push_back(partSelectionStrategy);
//	partSelectionStrategy->DoInitialize();

  Ptr<RequestSchedulingStrategyBase> requestSchedulingStrategy = Create<RequestSchedulingStrategyBase, Ptr<PushPullClient > > (client);
  strategyStore.push_back (requestSchedulingStrategy);
  requestSchedulingStrategy->DoInitialize ();

  Ptr<PushPullVideoMetricsBase> videoMetricsBase = Create<PushPullVideoMetricsBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (videoMetricsBase);
  videoMetricsBase->DoInitialize ();

  aPeerConnectorStrategy = peerConnectorStrategy;
}

void ProtocolFactory::CreateGiveToGetProtocol (Ptr<PushPullClient> client, std::list<Ptr<AbstractStrategy> > &strategyStore, Ptr<PeerConnectorStrategyBase>& aPeerConnectorStrategy)
{
  Ptr<PeerConnectorStrategyBase> peerConnectorStrategy = Create<PeerConnectorStrategyBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (peerConnectorStrategy);
  peerConnectorStrategy->DoInitialize ();

//	Ptr<GTGChokeUnChokeStrategy> chokeUnChokeStrategy = Create<GTGChokeUnChokeStrategy, Ptr<PushPullClient> >(client);
//	strategyStore.push_back(chokeUnChokeStrategy);
//	chokeUnChokeStrategy->DoInitialize();

//	Ptr<GTGPartSelectionStrategy> partSelectionStrategy = Create<GTGPartSelectionStrategy, Ptr<PushPullClient> >(client);
//	strategyStore.push_back(partSelectionStrategy);
//	partSelectionStrategy->DoInitialize();

  Ptr<RequestSchedulingStrategyBase> requestSchedulingStrategy = Create<RequestSchedulingStrategyBase, Ptr<PushPullClient > > (client);
  strategyStore.push_back (requestSchedulingStrategy);
  requestSchedulingStrategy->DoInitialize ();

  Ptr<PushPullVideoMetricsBase> videoMetricsBase = Create<PushPullVideoMetricsBase, Ptr<PushPullClient> > (client);
  strategyStore.push_back (videoMetricsBase);
  videoMetricsBase->DoInitialize ();

  aPeerConnectorStrategy = peerConnectorStrategy;
}

} // ns pushpull
} // ns ns3
