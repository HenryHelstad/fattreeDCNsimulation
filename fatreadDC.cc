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

int main(int argc, char *argv[]){
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
    pointtopoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointtopoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
   
 

 
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


    vector<NetDeviceContainer> devices;
    for(NodeContainer i : connections)
        devices.push_back(pointtopoint.Install(i));
 
    InternetStackHelper stack;
    stack.InstallAll ();
    
    Ipv4AddressHelper address;
    address.SetBase ("10.0.0.0", "255.255.255.0");

    vector<Ipv4InterfaceContainer> interfaces;
        
    for(NetDeviceContainer i : devices)
        interfaces.push_back( address.Assign (i) );


  
    /* 
    InternetStackHelper stack;
    stack.InstallAll ();
    
    TrafficControlHelper tchPfifo;
    tchPfifo.SetRootQueueDisc ("ns3::PfifoFastQueueDisc")
    
    TrafficControlHelper tchCoDel;
    tchCoDel.SetRootQueueDisc ("ns3::CoDelQueueDisc")
    
    Ipv4AddressHelper address;
    address.SetBase ("10.0.0.0", "255.255.255.0");

*/

    

}
