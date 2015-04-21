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
 * Authors: Rene Glebke
 * Contributors: Taejin Park
 */

#ifndef PUSHPULLVIDEOMETRICS_H_
#define PUSHPULLVIDEOMETRICS_H_

#include "AbstractStrategy.h"
#include "PushPullVideoClient.h"

#include "ns3/nstime.h"

#include <map>

namespace ns3 {
namespace pushpull {

/**
 * \ingroup PushPull
 *
 * \brief Gathers client-local information about a simulated PushPull-based Video-on-Demand (VOD) process.
 *
 * This class collects information from one client in a simulated PushPull Video-on-Demand swarm for further analysis or the implementation of (theoretical)
 * protocols that use statistical information for parameter adaption.
 *
 * The class as an example includes an implementation of the Fluency metric introduced by Huang et.al. in their <a href="http://dx.doi.org/10.1145/1402958.1403001" target="_blank">2008 SIGCOMM paper</a>.
 */
class PushPullVideoMetricsBase : public AbstractStrategy
{
// Fields
private:
  // The PushPullVideoClient this instance is associated with
  Ptr<PushPullVideoClient> m_myVideoClient;

  // Was a periodic position change announced? (Important for some metrics)
  bool m_periodicPositionChangeAnnounced;

  // Fluency metric
  Time m_bufferStart, m_bufferEnd, m_playbackStart, m_playbackEnd, m_playbackPosition, m_missedratePeriod;
  Time m_fluencyNumerator, m_fluencyDenominator;

// Constructors etc.
public:
  PushPullVideoMetricsBase (Ptr<PushPullClient> myClient);
  virtual ~PushPullVideoMetricsBase ();
  virtual void DoInitialize ();

// Event listeners etc.
public:
  void ProcessPlaybackPositionWillChangePeriodicallyEvent ();
  virtual void ProcessPlaybackPositionChangedEvent (Time newPosition);

  /**
   * \brief React to a change of the playback state of a client.
   */
  virtual void ProcessPlaybackStateChangedEvent ();

// Public methods for accessing the measured metrics
public:
  /**
   * \brief Return the collected periodic metrics.
   *
   * The up-to-dateness of the periodic metrics depends on their specific implementation.
   *
   * @returns a map containing the last values of the collected periodic metrics
   */
  std::map<std::string, std::string> ReturnPeriodicMetrics ();
};

} // ns pushpull
} // ns ns3

#endif /* PUSHPULLVIDEOMETRICS_H_ */
