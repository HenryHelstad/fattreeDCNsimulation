#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/olsr-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/csma-module.h"
#include "ns3/stats-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"

using namespace ns3;
using namespace std;

static const float THROUGHPUT = 100; //idk 

//bunch of helper functions for tracing output
static void
CwndTracer (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd)
    {
      *cWndStream->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd = false;
    }
  *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue = newval;

  if (!firstSshThr)
    {
      *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue << std::endl;
    }
}

static void
SsThreshTracer (uint32_t oldval, uint32_t newval)
{
  if (firstSshThr)
    {
      *ssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr = false;
    }
  *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue = newval;

  if (!firstCwnd)
    {
      *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue << std::endl;
    }
}

static void
RttTracer (Time oldval, Time newval)
{
  if (firstRtt)
    {
      *rttStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt = false;
    }
  *rttStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RtoTracer (Time oldval, Time newval)
{
  if (firstRto)
    {
      *rtoStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRto = false;
    }
  *rtoStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
NextTxTracer (SequenceNumber32 old, SequenceNumber32 nextTx)
{
  NS_UNUSED (old);
  *nextTxStream->GetStream () << Simulator::Now ().GetSeconds () << " " << nextTx << std::endl;
}

static void
InFlightTracer (uint32_t old, uint32_t inFlight)
{
  NS_UNUSED (old);
  *inFlightStream->GetStream () << Simulator::Now ().GetSeconds () << " " << inFlight << std::endl;
}

static void
NextRxTracer (SequenceNumber32 old, SequenceNumber32 nextRx)
{
  NS_UNUSED (old);
  *nextRxStream->GetStream () << Simulator::Now ().GetSeconds () << " " << nextRx << std::endl;
}

static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
}

static void
TraceSsThresh (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer));
}

static void
TraceRtt (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer));
}

static void
TraceRto (std::string rto_tr_file_name)
{
  AsciiTraceHelper ascii;
  rtoStream = ascii.CreateFileStream (rto_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/RTO", MakeCallback (&RtoTracer));
}

static void
TraceNextTx (std::string &next_tx_seq_file_name)
{
  AsciiTraceHelper ascii;
  nextTxStream = ascii.CreateFileStream (next_tx_seq_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/NextTxSequence", MakeCallback (&NextTxTracer));
}

static void
TraceInFlight (std::string &in_flight_file_name)
{
  AsciiTraceHelper ascii;
  inFlightStream = ascii.CreateFileStream (in_flight_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/BytesInFlight", MakeCallback (&InFlightTracer));
}


static void
TraceNextRx (std::string &next_rx_seq_file_name)
{
  AsciiTraceHelper ascii;
  nextRxStream = ascii.CreateFileStream (next_rx_seq_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/1/RxBuffer/NextRxSequence", MakeCallback (&NextRxTracer));
}




///////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////
int main(int argc, char *argv[]){


  //just basic variables
  std::string transport_prot = "TcpNewReno";
  double error_p = 0.0;
  std::string bandwidth = "2Mbps";
  std::string delay = "0.01ms";
  std::string access_bandwidth = "10Mbps";
  std::string access_delay = "45ms";
  bool tracing = true;
  std::string prefix_file_name = "TcpVariantsComparison";
  uint64_t data_mbytes = 0;
  uint32_t mtu_bytes = 400;
  uint16_t num_flows = 1;
  double duration = 10.0;
  uint32_t run = 0;
  bool flow_monitor = false;
  bool pcap = true;
  bool sack = true;
  std::string queue_disc_type = "ns3::PfifoFastQueueDisc";
  std::string recovery = "ns3::TcpClassicRecovery";
  
  //stuff for command line args if we need, dont think we will
  CommandLine cmd;
  cmd.AddValue ("transport_prot", "Transport protocol to use: TcpNewReno, "
                "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat, "
		"TcpLp", transport_prot);
  cmd.AddValue ("error_p", "Packet error rate", error_p);
  cmd.AddValue ("bandwidth", "Bottleneck bandwidth", bandwidth);
  cmd.AddValue ("delay", "Bottleneck delay", delay);
  cmd.AddValue ("access_bandwidth", "Access link bandwidth", access_bandwidth);
  cmd.AddValue ("access_delay", "Access link delay", access_delay);
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
  cmd.AddValue ("prefix_name", "Prefix of output trace file", prefix_file_name);
  cmd.AddValue ("data", "Number of Megabytes of data to transmit", data_mbytes);
  cmd.AddValue ("mtu", "Size of IP packets to send in bytes", mtu_bytes);
  cmd.AddValue ("num_flows", "Number of flows", num_flows);
  cmd.AddValue ("duration", "Time to allow flows to run in seconds", duration);
  cmd.AddValue ("run", "Run index (for setting repeatable seeds)", run);
  cmd.AddValue ("flow_monitor", "Enable flow monitor", flow_monitor);
  cmd.AddValue ("pcap_tracing", "Enable or disable PCAP tracing", pcap);
  cmd.AddValue ("queue_disc_type", "Queue disc type for gateway (e.g. ns3::CoDelQueueDisc)", squeue_disc_type);
  cmd.AddValue ("sack", "Enable or disable SACK option", sack);
  cmd.AddValue ("recovery", "Recovery algorithm type to use (e.g., ns3::TcpPrrRecovery", recovery);
  cmd.Parse (argc, argv);
  
  
  //expand tcp variant name
  transport_prot = std::string ("ns3::") + transport_prot;
  
  //seed rng
  SeedManager::SetSeed (1);
  SeedManager::SetRun (run);
  
  // Set the simulation start and stop time
  double start_time = 0.1;
  double stop_time = start_time + duration;

  // 4 MB of TCP buffer
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1 << 21));
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1 << 21));
  Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (sack));

  Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType",
                      TypeIdValue (TypeId::LookupByName (recovery)));
  // Select TCP variant
  if (transport_prot.compare ("ns3::TcpWestwoodPlus") == 0)
    { 
      // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      // the default protocol type in ns3::TcpWestwood is WESTWOOD
      Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
    }
  else
    {
      TypeId tcpTid;
      NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (transport_prot, &tcpTid), "TypeId " << transport_prot << " not found");
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transport_prot)));
    }
    
    
  // Configure the error model
  // Here we use RateErrorModel with packet error rate
  Ptr<ExponentialRandomVariable> uv = CreateObject<ExponentialRandomVariable> ();
  uv->SetStream (50);
  RateErrorModel error_model;
  error_model.SetRandomVariable (uv);
  error_model.SetUnit (RateErrorModel::ERROR_UNIT_PACKET);
  error_model.SetRate (error_p);
  
  //set queue sizes
  DataRate access_b (access_bandwidth);
  DataRate bottle_b (bandwidth);
  Time access_d (access_delay);
  Time bottle_d (delay);

  uint32_t size = static_cast<uint32_t>((std::min (access_b, bottle_b).GetBitRate () / 8) *
    ((access_d + bottle_d) * 2).GetSeconds ());

  Config::SetDefault ("ns3::PfifoFastQueueDisc::MaxSize",
                      QueueSizeValue (QueueSize (QueueSizeUnit::PACKETS, size / mtu_bytes)));
  Config::SetDefault ("ns3::CoDelQueueDisc::MaxSize",
                      QueueSizeValue (QueueSize (QueueSizeUnit::BYTES, size)));

  /*
   // Ipv4StaticRoutingHelper staticRouting;
   // Ipv4ListRoutingHelper listRouting; 
   // Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue(210));
   // Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));

    //initalizing containers and setting up connections
    
    pointtopointhelper pointtopoint;
    pointtopoint.setdeviceattribute ("datarate", stringvalue ("5mbps"));
    pointtopoint.setchannelattribute ("delay", stringvalue ("2ms"));
    netdevicecontainer devices;

    NodeContainer nodes;
    nodes.Create (18);

    vector<NetDeviceContainer> NETDEVICE;
    NETDEVICE.reserve(20);

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[0] , NETDEVICE[2] ); 
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[0] , NETDEVICE[4] );
    
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[1] , NETDEVICE[3] );
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[1] , NETDEVICE[5] );

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[2] , NETDEVICE[6] );
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[2] , NETDEVICE[7] );

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[3] , NETDEVICE[6] );
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[3] , NETDEVICE[7] );
 
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[4] , NETDEVICE[8] );
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[4] , NETDEVICE[9] );

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[5] , NETDEVICE[8] );
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[5] , NETDEVICE[9] );

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[6] , NETDEVICE[10] );  
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[6] , NETDEVICE[11] );

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[7] , NETDEVICE[12] );
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[7] , NETDEVICE[13] );

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[8] , NETDEVICE[14] );
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[8] , NETDEVICE[15] );

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[9] , NETDEVICE[16] );
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE[9] , NETDEVICE[17] );
  */
 

    PointToPointHelper pointtopoint;
    pointtopoint.SetDeviceAttribute ("DataRate", StringValue (access_bandwidth));
    pointtopoint.SetChannelAttribute ("Delay", StringValue (access_delay));
    pointtopoint.SetDeviceAttribute ("ReceiveErrorModel", PointerValue (&error_model));
 

 
    NodeContainer n1a, n1b, n2a, n2b, n2c, n2d, n3a, n3b, n3c, n3d;
    NodeContainer l1,l2,l3,l4,l5,l6,l7,l8;
    n1a.Create(1);
    n1b.Create(1); 
    n2a.Create(1); 
    n2b.Create(1); 
    n2c.Create(1); 
    n2d.Create(1); 
    n3a.Create(1); 
    n3b.Create(1); 
    n3c.Create(1); 
    n3d.Create(1);
    l1.Create(1);
    l2.Create(1);
    l3.Create(1);
    l4.Create(1);
    l5.Create(1);
    l6.Create(1);
    l7.Create(1);
    l8.Create(1);
    

    vector<NodeContainer> connections;
    
    connections.push_back(NodeContainer (n1a, n2a));    
    connections.push_back(NodeContainer (n1a, n2b));
    connections.push_back(NodeContainer (n1a, n2c));

    connections.push_back(NodeContainer (n1b, n2b));
    connections.push_back(NodeContainer (n1b, n2c));
    connections.push_back(NodeContainer (n1b, n2d));

    connections.push_back(NodeContainer (n2a, n3a));
    connections.push_back(NodeContainer (n2a, n3b));
    connections.push_back(NodeContainer (n2b, n3a));
    connections.push_back(NodeContainer (n2b, n3b));
    connections.push_back(NodeContainer (n2c, n3c));
    connections.push_back(NodeContainer (n2c, n3d));
    connections.push_back(NodeContainer (n2d, n3c));
    connections.push_back(NodeContainer (n2d, n3d));
    connections.push_back(NodeContainer (n3a, l1));
    connections.push_back(NodeContainer (n3a, l2));
    connections.push_back(NodeContainer (n3b, l3));
    connections.push_back(NodeContainer (n3b, l4));
    connections.push_back(NodeContainer (n3c, l5));
    connections.push_back(NodeContainer (n3c, l6));
    connections.push_back(NodeContainer (n3d, l7));
    connections.push_back(NodeContainer (n3d, l8));
    
    
    

  //more traffic control configuration
  TrafficControlHelper tchPfifo;
  tchPfifo.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");

  TrafficControlHelper tchCoDel;
  tchCoDel.SetRootQueueDisc ("ns3::CoDelQueueDisc");



    vector<NetDeviceContainer> devices;
    for(NodeContainer i : connections)
        devices.push_back(pointtopoint.Install(i));
 
    InternetStackHelper stack;
    stack.InstallAll ();
    
    Ipv4AddressHelper address;
    address.SetBase ("10.0.0.0", "255.255.255.0");

    Ipv4InterfaceContainer sink_interfaces;

    vector<Ipv4InterfaceContainer> interfaces;
        
    for(NetDeviceContainer i : devices)
        tchPfifo.Install (i);
        
    for(NetDeviceContainer i : devices){
        interfaces.push_back( address.Assign (i) );
        address.NewNetwork();
    }
    
    sink_interfaces.Add(interfaces.Get(13));    

    //Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    
    
    
   
 //   InternetStackHelper internet;
     
//    internet.Install (connection_1a_2a);
//    internet.Install (connection_1a_2b);
//    internet.Install (connection_1a_2c);
//
//    internet.Install (connection_1b_2b);
//    internet.Install (connection_1b_2c);
//    internet.Install (connection_1b_2d);
//






  // Set up tracing if enabled
  if (tracing)
    {
      std::ofstream ascii;
      Ptr<OutputStreamWrapper> ascii_wrap;
      ascii.open ((prefix_file_name + "-ascii").c_str ());
      ascii_wrap = new OutputStreamWrapper ((prefix_file_name + "-ascii").c_str (),
                                            std::ios::out);
      stack.EnableAsciiIpv4All (ascii_wrap);

      Simulator::Schedule (Seconds (0.00001), &TraceCwnd, prefix_file_name + "-cwnd.data");
      Simulator::Schedule (Seconds (0.00001), &TraceSsThresh, prefix_file_name + "-ssth.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRtt, prefix_file_name + "-rtt.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRto, prefix_file_name + "-rto.data");
      Simulator::Schedule (Seconds (0.00001), &TraceNextTx, prefix_file_name + "-next-tx.data");
      Simulator::Schedule (Seconds (0.00001), &TraceInFlight, prefix_file_name + "-inflight.data");
      Simulator::Schedule (Seconds (0.1), &TraceNextRx, prefix_file_name + "-next-rx.data");
    }

  if (pcap)
    {
      UnReLink.EnablePcapAll (prefix_file_name, true);
      LocalLink.EnablePcapAll (prefix_file_name, true);
    }

  // Flow monitor
  FlowMonitorHelper flowHelper;
  if (flow_monitor)
    {
      flowHelper.InstallAll ();
    }

  Simulator::Stop (Seconds (stop_time));
  Simulator::Run ();

  if (flow_monitor)
    {
      flowHelper.SerializeToXmlFile (prefix_file_name + ".flowmonitor", true, true);
    }

  Simulator::Destroy ();
  return 0;


    

}
