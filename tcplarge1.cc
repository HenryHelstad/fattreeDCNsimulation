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
// Network topology: OLD, NOT CURRENT
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
 
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
 
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
  //std::cout<<"time: " <<Simulator::Now ().GetSeconds ();
  //std::cout<<" " <<oldval << " " << newval <<std::endl;
  
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
    
   // OLD NETWORK: 


   ///  n0 --- n1 ---- n2 ---- n3
   ///  n4 --- |        | ---- n5
    
 
  // Here, we will explicitly create three nodes.  The first container contains
  // nodes 0 and 1 from the diagram above, and the second one contains nodes
  // 1 and 2.  This reflects the channel connectivity, and will be used to
  // install the network interfaces and connect them with a channel.
  /*NodeContainer n0n1;
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
  n5n1.Create (1);*/


  ///////////////////////////////////////////////////////

  // NEW NETWORK:
  /*
  //                    n16    n17
  //                    / \    /\
  //                  /    \ /    \
  //                /      / \      \
  //              /      /     \      \  
  //            /      /         \      \
  //          /      /             \      \
  //        n6     n7              n14    n15 
  //       |  \  / |              |   \ /   |  
  //       |   /\  |              |   / \   |
  //       | /   \ |              | /    \  |
  //      n2       n5            n10       n13 
  //    /   \     / \           / \       /  \
  //  n0   n1   n3   n4       n8   n9   n11   n12
  */
  ///////////////////////////////////////////////////////

  NodeContainer n0n2;
  n0n2.Create(2);

  NodeContainer n1n2;
  n1n2.Create(1);
  n1n2.Add(n0n2.Get(1));

  NodeContainer n3n5;
  n3n5.Create(2);

  NodeContainer n4n5;
  n4n5.Create(1);
  n4n5.Add(n3n5.Get(1));

  NodeContainer n2n6;
  n2n6.Add(n0n2.Get(1));
  n2n6.Create(1);

  NodeContainer n2n7;
  n2n7.Add(n0n2.Get(1));
  n2n7.Create(1);

  NodeContainer n5n6;
  n5n6.Add(n3n5.Get(1));
  n5n6.Add(n2n6.Get(1));
  
  NodeContainer n5n7;
  n5n7.Add(n3n5.Get(1));
  n5n7.Add(n2n7.Get(1));

  NodeContainer n6n16;
  n6n16.Add(n2n6.Get(1));
  n6n16.Create(1);

  NodeContainer n7n17;
  n7n17.Add(n2n7.Get(1));
  n7n17.Create(1);

  // END of left half of links

  NodeContainer n8n10;
  n8n10.Create(2);

  NodeContainer n9n10;
  n9n10.Create(1);
  n9n10.Add(n8n10.Get(1));

  NodeContainer n11n13;
  n11n13.Create(2);

  NodeContainer n12n13;
  n12n13.Create(1);
  n12n13.Add(n11n13.Get(1));

  NodeContainer n10n14;
  n10n14.Add(n8n10.Get(1));
  n10n14.Create(1);

  NodeContainer n10n15;
  n10n15.Add(n8n10.Get(1));
  n10n15.Create(1);

  NodeContainer n13n14;
  n13n14.Add(n11n13.Get(1));
  n13n14.Add(n10n14.Get(1));

  NodeContainer n13n15;
  n13n15.Add(n11n13.Get(1));
  n13n15.Add(n10n15.Get(1));

  NodeContainer n14n16;
  n14n16.Add(n10n14.Get(1));
  n14n16.Add(n6n16.Get(1));

  NodeContainer n15n17;
  n15n17.Add(n10n15.Get(1));
  n15n17.Add(n7n17.Get(1));

  //END of right half of links

  //END of main network definitiion
  ///////////////////////////////////////////////

  //random number generator seeding

  srand(time(0));
  SeedManager::SetSeed (rand());
  SeedManager::SetRun (0);
  
  //random packet loss
  Ptr<ExponentialRandomVariable> uv = CreateObject<ExponentialRandomVariable> ();
  uv->SetStream (50);
  RateErrorModel error_model;
  error_model.SetRandomVariable (uv);
  error_model.SetUnit (RateErrorModel::ERROR_UNIT_PACKET);
  error_model.SetRate (.005);
 
  ////////////////////////////////////////////////////////////////
  //Setting up point to point helper for each link
  ////////////////////////////////////////////////////////////////

  // We create the channels first without any IP addressing information
  // First make and configure the helper, so that it will put the appropriate
  // attributes on the network interfaces and channels we are about to install.
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (10000000)));
  p2p.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  p2p.SetDeviceAttribute ("ReceiveErrorModel", PointerValue (&error_model));
 
  // And then install devices and channels connecting our topology.
  NetDeviceContainer d0 = p2p.Install (n0n2);
  NetDeviceContainer d1 = p2p.Install (n1n2);
  NetDeviceContainer d2 = p2p.Install (n3n5);
  NetDeviceContainer d3 = p2p.Install (n4n5);
  NetDeviceContainer d4 = p2p.Install (n2n6);
  NetDeviceContainer d5 = p2p.Install (n2n7);
  NetDeviceContainer d6 = p2p.Install (n5n6);
  NetDeviceContainer d7 = p2p.Install (n5n7);
  NetDeviceContainer d8 = p2p.Install (n6n16);
  NetDeviceContainer d9 = p2p.Install (n7n17);
  //end of left half of links
  NetDeviceContainer d10 = p2p.Install (n8n10);
  NetDeviceContainer d11 = p2p.Install (n9n10);
  NetDeviceContainer d12 = p2p.Install (n11n13);
  NetDeviceContainer d13 = p2p.Install (n12n13);
  NetDeviceContainer d14 = p2p.Install (n10n14);
  NetDeviceContainer d15 = p2p.Install (n10n15);
  NetDeviceContainer d16 = p2p.Install (n13n14);
  NetDeviceContainer d17 = p2p.Install (n13n15);
  NetDeviceContainer d18 = p2p.Install (n14n16);
  NetDeviceContainer d19 = p2p.Install (n15n17);
  //end of right half of links
 
  ///////////////////////////////////////////////////////////////////////
  //End of point to point helper stuff, now define TCP Variant
  //////////////////////////////////////////////////////////////////////

  //Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
  //Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpDctcp::GetTypeId ())); 
  
  
  ////////////////////////////////////////////////////////////////////////
  ///
  //  THIS IS THE STRING TO CHANGE TCP VARIANT, ---------------------|
  //                                                                 v
  //////////////////////////////////////////////////////////////////////////////////
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
  ////////////////////////////////////////////////////////////////////////////////////
  
  
  
 
 
  // Now add ip/tcp stack to all nodes.
  InternetStackHelper internet;
  internet.InstallAll ();

  /////////////////////////////////////////////////////////////////////
  //ADDING IP ADDRESSES
  //////////////////////////////////////////////////////////////////////
 
  // Later, we add IP addresses.
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.0.0", "255.255.255.0");
  //assign to variable to initite source flow later (main source)
  Ipv4InterfaceContainer ipInterfs0 = ipv4.Assign (d0);
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  //n1 is secondary soruce
  Ipv4InterfaceContainer ipInterfs1 = ipv4.Assign (d1);
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  ipv4.Assign (d2);
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  ipv4.Assign (d3);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  ipv4.Assign (d4);
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  ipv4.Assign (d5);
  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  ipv4.Assign (d6);
  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  ipv4.Assign (d7);
  ipv4.SetBase ("10.1.8.0", "255.255.255.0");
  ipv4.Assign (d8);
  ipv4.SetBase ("10.1.9.0", "255.255.255.0");
  ipv4.Assign (d9);
  //end of left link IP addresses 
  ipv4.SetBase ("10.1.10.0", "255.255.255.0");
  ipv4.Assign (d10);
  ipv4.SetBase ("10.1.11.0", "255.255.255.0");
  ipv4.Assign (d11);
  ipv4.SetBase ("10.1.12.0", "255.255.255.0");
  ipv4.Assign (d12);
  ipv4.SetBase ("10.1.13.0", "255.255.255.0");
  ipv4.Assign (d13);
  ipv4.SetBase ("10.1.14.0", "255.255.255.0");
  ipv4.Assign (d14);
  ipv4.SetBase ("10.1.15.0", "255.255.255.0");
  ipv4.Assign (d15);
  ipv4.SetBase ("10.1.16.0", "255.255.255.0");
  ipv4.Assign (d16);
  ipv4.SetBase ("10.1.17.0", "255.255.255.0");
  ipv4.Assign (d17);
  ipv4.SetBase ("10.1.18.0", "255.255.255.0");
  ipv4.Assign (d18);
  ipv4.SetBase ("10.1.19.0", "255.255.255.0");
  ipv4.Assign (d19);
  //end of right half link IP addresses

  //////////////////////////////////////////////////////////////
  //END OF ADDING IP ADDRESSES
  //////////////////////////////////////////////////////////////
  
  //ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  //Ipv4InterfaceContainer ipInterfs0 = ipv4.Assign (dev2);
  //ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  //Ipv4InterfaceContainer ipInterfs2 = ipv4.Assign (dev4);
 
  // and setup ip routing tables to get total ip-level connectivity.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  // Simulation 1
  //
  // Send 2000000 bytes over a connection to server port 50000 at time 0
  // Should observe SYN exchange, a lot of data segments and ACKS, and FIN 
  // exchange.  FIN exchange isn't quite compliant with TCP spec (see release
  // notes for more info)
  //
 

  ////////////////////////////////////////////////////////////////////////
  //SET UP SINKS
  //
  //main sink is n12 on link n12n13 on device d13
  ////////////////////////////////////////////////////////////////////////

  uint16_t servPort0 = 50000;
  uint16_t servPort1 = 50001;
 
  // Create a packet sink to receive these packets on n12...
  PacketSinkHelper sink0 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), servPort0));
 
  ApplicationContainer apps0 = sink0.Install (n12n13.Get(0));
  apps0.Start (Seconds (0.0));
  apps0.Stop (Seconds (100.0));
 
  //second sink on n11
  PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), servPort1));
  ApplicationContainer apps1 = sink1.Install (n11n13.Get(0));
  apps1.Start (Seconds (0.0));
  apps1.Stop (Seconds (100.0));                 
 
  ////////////////////////////////////////////////////////////////////////
  //END OF SINK SETUP
  ////////////////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////////////////
  //SETUP SOURCE SOCKETS AND INITIALIZE FLOWS
  ////////////////////////////////////////////////////////////////////////
 
  // Create a source to send packets from n0.  Instead of a full Application
  // and the helper APIs you might see in other example files, this example
  // will use sockets directly and register some socket callbacks as a sending
  // "Application".
 
  // Create and bind the socket for main sorce, n0
  Ptr<Socket> localSocket0 =
    Socket::CreateSocket (n0n2.Get (0), TcpSocketFactory::GetTypeId ());
  localSocket0->Bind ();

  //create socket for secondary source on n1
  Ptr<Socket> localSocket1 =
    Socket::CreateSocket (n1n2.Get (0), TcpSocketFactory::GetTypeId ());
  localSocket1->Bind ();
 
  // Trace changes to the congestion window
  //DO NOT NEED
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
 
  // ...and schedule the sending "Application"; This is similar to what an 
  // ns3::Application subclass would do internally.

  //MAIN SOURCE n0
  Simulator::ScheduleNow (&StartFlow, localSocket0,
                          ipInterfs0.GetAddress (1), servPort0);

  //secondary source n1
  Simulator::ScheduleNow (&StartFlow, localSocket1,
                          ipInterfs1.GetAddress (1), servPort1);
 
  // One can toggle the comment for the following line on or off to see the
  // effects of finite send buffer modelling.  One can also change the size of
  // said buffer.
 
  //localSocket->SetAttribute("SndBufSize", UintegerValue(4096));
  
  ////////////////////////////////////////////////////////
  //Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (n0n1.Get (0), TcpSocketFactory::GetTypeId ());
 
  //Ask for ASCII and pcap traces of network traffic
  AsciiTraceHelper ascii;
  p2p.EnableAsciiAll (ascii.CreateFileStream ("tcplarge.tr"));
  p2p.EnablePcapAll ("tcplarge");
  
  /////////////////////////////////////////////////////////////////////
  //Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("tcplarge1.cwnd");
  //ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
  //Simulator::Schedule (Seconds (0.00001), &TraceCwnd, "tcplarge11-cwnd.data");
  
 
  // Finally, set up the simulator to run.  The 1000 second hard limit is a
  // failsafe in case some change above causes the simulation to never end
  Simulator::Stop (Seconds (100));
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
