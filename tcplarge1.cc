/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */
 
//
// Network topology
//
//           10Mb/s, 10ms       10Mb/s, 10ms
//       n0-----------------n1-----------------n2
//
//
// - Tracing of queues and packet receptions to file 
//   "tcp-large-transfer.tr"
// - pcap traces also generated in the following files
//   "tcp-large-transfer-$n-$i.pcap" where n and i represent node and interface
// numbers respectively
//  Usage (e.g.): ./ns3 run tcp-large-transfer
 
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
 
#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/stats-module.h"
#include "ns3/error-model.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
//#include "ns3/tcp-dctcp.h"
 
using namespace ns3;
 
NS_LOG_COMPONENT_DEFINE ("TcpLargeTransfer");


//static bool firstCwnd = true;
//static Ptr<OutputStreamWrapper> cWndStream;
//static Ptr<OutputStreamWrapper> ssThreshStream;
//static uint32_t cWndValue;
//static uint32_t ssThreshValue;
 
// The number of bytes to send in this simulation.
static const uint32_t totalTxBytes = 2000000;
static uint32_t currentTxBytes = 0;
// Perform series of 1040 byte writes (this is a multiple of 26 since
// we want to detect data splicing in the output stream)
static const uint32_t writeSize = 1040;
uint8_t data[writeSize];
 
// These are for starting the writing process, and handling the sending 
// socket's notification upcalls (events).  These two together more or less
// implement a sending "Application", although not a proper ns3::Application
// subclass.
 
void StartFlow (Ptr<Socket>, Ipv4Address, uint16_t);
void WriteUntilBufferFull (Ptr<Socket>, uint32_t);
 
static void 
CwndTracer (uint32_t oldval, uint32_t newval)
{
  NS_LOG_INFO ("Moving cwnd from " << oldval << " to " << newval);
  std::cout<<oldval << " " << newval <<std::endl;
}

/*static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}*/

 
int main (int argc, char *argv[])
{
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
  //  LogComponentEnable("TcpL4Protocol", LOG_LEVEL_ALL);
  //  LogComponentEnable("TcpSocketImpl", LOG_LEVEL_ALL);
  //  LogComponentEnable("PacketSink", LOG_LEVEL_ALL);
  //  LogComponentEnable("TcpLargeTransfer", LOG_LEVEL_ALL);
 
  //CommandLine cmd (__FILE__);
  //cmd.Parse (argc, argv);
 
  // initialize the tx buffer.
  for(uint32_t i = 0; i < writeSize; ++i)
    {
      char m = toascii (97 + i % 26);
      data[i] = m;
    }
    
    
   ///  n0 --- n1 ---- n2 ---- n3
   ///  n4 --- |        | ---- n5
    
 
  // Here, we will explicitly create three nodes.  The first container contains
  // nodes 0 and 1 from the diagram above, and the second one contains nodes
  // 1 and 2.  This reflects the channel connectivity, and will be used to
  // install the network interfaces and connect them with a channel.
  NodeContainer n0n1;
  n0n1.Create (2);
 
  NodeContainer n1n2;
  n1n2.Add (n0n1.Get (1));
  n1n2.Create (1);
  
  NodeContainer n2n3;
  n2n3.Add (n1n2.Get (1));
  n2n3.Create (1);
  
  //n4
  NodeContainer n4n1;
  n4n1.Add(n0n1.Get (1));
  n4n1.Create (1);
  
  //n5
  NodeContainer n5n1;
  n5n1.Add(n1n2.Get (1));
  n5n1.Create (1);
 
  // We create the channels first without any IP addressing information
  // First make and configure the helper, so that it will put the appropriate
  // attributes on the network interfaces and channels we are about to install.
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (10000000)));
  p2p.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
 
  // And then install devices and channels connecting our topology.
  NetDeviceContainer dev0 = p2p.Install (n0n1);
  NetDeviceContainer dev1 = p2p.Install (n1n2);
  NetDeviceContainer dev2 = p2p.Install (n2n3);
  NetDeviceContainer dev3 = p2p.Install (n4n1);
  NetDeviceContainer dev4 = p2p.Install (n5n1);
 
  //////////////////////////////////////
  //Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
  //Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpDctcp::GetTypeId ())); 
  
  
  ////////////////////////////////////////////////////////////////////////
  ///
  //  THIS IS THE STRING TO CHANGE TCP VARIANT, ---------------------|
  //                                                                 v
  //////////////////////////////////////////////////////////////////////////////////
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpVegas"));
  ////////////////////////////////////////////////////////////////////////////////////
  
  
  
 
 
  // Now add ip/tcp stack to all nodes.
  InternetStackHelper internet;
  internet.InstallAll ();
 
  // Later, we add IP addresses.
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  ipv4.Assign (dev0);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  ipv4.Assign (dev1);
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  ipv4.Assign (dev3);
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer ipInterfs = ipv4.Assign (dev2);
  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer ipInterfs2 = ipv4.Assign (dev4);
 
  // and setup ip routing tables to get total ip-level connectivity.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  // Simulation 1
  //
  // Send 2000000 bytes over a connection to server port 50000 at time 0
  // Should observe SYN exchange, a lot of data segments and ACKS, and FIN 
  // exchange.  FIN exchange isn't quite compliant with TCP spec (see release
  // notes for more info)
  //
 
  uint16_t servPort = 50000;
 
  // Create a packet sink to receive these packets on n2...
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), servPort));
 
  ApplicationContainer apps = sink.Install (n2n3.Get (1));
  apps.Start (Seconds (0.0));
  apps.Stop (Seconds (5.0));
 
  ///////////////////////////////////////////////////////////
  PacketSinkHelper sink2 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), servPort));
  ApplicationContainer apps2 = sink2.Install (n5n1.Get (1));
  apps2.Start (Seconds (0.0));
  apps2.Stop (Seconds (5.0));                 
 
 
  // Create a source to send packets from n0.  Instead of a full Application
  // and the helper APIs you might see in other example files, this example
  // will use sockets directly and register some socket callbacks as a sending
  // "Application".
 
  // Create and bind the socket...
  Ptr<Socket> localSocket =
    Socket::CreateSocket (n0n1.Get (0), TcpSocketFactory::GetTypeId ());
  localSocket->Bind ();
 
  // Trace changes to the congestion window
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
 
  // ...and schedule the sending "Application"; This is similar to what an 
  // ns3::Application subclass would do internally.
  Simulator::ScheduleNow (&StartFlow, localSocket,
                          ipInterfs.GetAddress (1), servPort);
 
  // One can toggle the comment for the following line on or off to see the
  // effects of finite send buffer modelling.  One can also change the size of
  // said buffer.
 
  //localSocket->SetAttribute("SndBufSize", UintegerValue(4096));
  
  ////////////////////////////////////////////////////////
  //Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (n0n1.Get (0), TcpSocketFactory::GetTypeId ());
 
  //Ask for ASCII and pcap traces of network traffic
  AsciiTraceHelper ascii;
  p2p.EnableAsciiAll (ascii.CreateFileStream ("tcplarge1.tr"));
  p2p.EnablePcapAll ("tcplarge1");
  
  /////////////////////////////////////////////////////////////////////
  //Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("tcplarge1.cwnd");
  //ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
  //Simulator::Schedule (Seconds (0.00001), &TraceCwnd, "tcplarge11-cwnd.data");
  
 
  // Finally, set up the simulator to run.  The 1000 second hard limit is a
  // failsafe in case some change above causes the simulation to never end
  Simulator::Stop (Seconds (1000));
  Simulator::Run ();
  Simulator::Destroy ();
}
 
 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//begin implementation of sending "Application"
void StartFlow (Ptr<Socket> localSocket,
                Ipv4Address servAddress,
                uint16_t servPort)
{
  NS_LOG_LOGIC ("Starting flow at time " <<  Simulator::Now ().GetSeconds ());
  localSocket->Connect (InetSocketAddress (servAddress, servPort)); //connect
 
  // tell the tcp implementation to call WriteUntilBufferFull again
  // if we blocked and new tx buffer space becomes available
  localSocket->SetSendCallback (MakeCallback (&WriteUntilBufferFull));
  WriteUntilBufferFull (localSocket, localSocket->GetTxAvailable ());
}
 
void WriteUntilBufferFull (Ptr<Socket> localSocket, uint32_t txSpace)
{
  while (currentTxBytes < totalTxBytes && localSocket->GetTxAvailable () > 0) 
    {
      uint32_t left = totalTxBytes - currentTxBytes;
      uint32_t dataOffset = currentTxBytes % writeSize;
      uint32_t toWrite = writeSize - dataOffset;
      toWrite = std::min (toWrite, left);
      toWrite = std::min (toWrite, localSocket->GetTxAvailable ());
      int amountSent = localSocket->Send (&data[dataOffset], toWrite, 0);
      if(amountSent < 0)
        {
          // we will be called again when new tx space becomes available.
          return;
        }
      currentTxBytes += amountSent;
    }
  if (currentTxBytes >= totalTxBytes)
    {
      localSocket->Close ();
    }
}