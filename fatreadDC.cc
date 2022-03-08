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

using namespace ns3;
using namespace std;

static const float THROUGHPUT = 100; //idk 

int main(int argc, char *argv[]){

   // Ipv4StaticRoutingHelper staticRouting;
   // Ipv4ListRoutingHelper listRouting; 
   // Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue(210));
   // Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));

    //initalizing containers and setting up connections
    
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
    NetDeviceContainer devices;

    NodeContainer nodes;
    nodes.Create (18);

    vector<NetDeviceContainer> NETDEVICE;
    NETDEVICE.reserve(20)

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(0) , NETDEVICE.Get(2) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(0) , NETDEVICE.Get(4) )
    
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(1) , NETDEVICE.Get(3) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(1) , NETDEVICE.Get(5) )

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(2) , NETDEVICE.Get(6) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(2) , NETDEVICE.Get(7) )

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(3) , NETDEVICE.Get(6) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(3) , NETDEVICE.Get(7) )
 
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(4) , NETDEVICE.Get(8) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(4) , NETDEVICE.Get(9) )

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(5) , NETDEVICE.Get(8) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(5) , NETDEVICE.Get(9) )

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(6) , NETDEVICE.Get(10) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(6) , NETDEVICE.Get(11) )

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(7) , NETDEVICE.Get(12) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(7) , NETDEVICE.Get(13) )

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(8) , NETDEVICE.Get(14) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(8) , NETDEVICE.Get(15) )

    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(2) , NETDEVICE.Get(16) )
    NETDEVICE.push_back (pointToPoint.Install( NETDEVICE.Get(2) , NETDEVICE.Get(17) )

 

 
 
 
 

 
    /*    NodeContainer n1a, n1b, n2a, n2b, n2c, n2d, n3a, n3b, n3c, n3d;
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

    NodeContainer connection_1a_2a = NodeContainer (n1a, n2a);
    NodeContainer connection_1a_2b = NodeContainer (n1a, n2b);
    NodeContainer connection_1a_2c = NodeContainer (n1a, n2c);

    NodeContainer connection_1b_2b = NodeContainer (n1b, n2b);
    NodeContainer connection_1b_2c = NodeContainer (n1b, n2c);
    NodeContainer connection_1b_2d = NodeContainer (n1b, n2d);

    NodeContainer connection_2a_3a = NodeContainer (n2a, n3a);
    NodeContainer connection_2a_3b = NodeContainer (n2a, n3b);
        
    NodeContainer connection_2b_3a = NodeContainer (n2b, n3a);
    NodeContainer connection_2b_3b = NodeContainer (n2b, n3b);
    
    NodeContainer connection_2c_3c = NodeContainer (n2c, n3c);
    NodeContainer connection_2c_3d = NodeContainer (n2c, n3d);
    
    NodeContainer connection_2d_3c = NodeContainer (n2d, n3c);
    NodeContainer connection_2d_3d = NodeContainer (n2d, n3d);
    
    NodeContainer connection_3a_l1 = NodeContainer (n3a, l1);
    NodeContainer connection_3a_l2 = NodeContainer (n3a, l2);
            
    NodeContainer connection_3b_l3 = NodeContainer (n3b, l3);
    NodeContainer connection_3b_l4 = NodeContainer (n3b, l4);
    
    NodeContainer connection_3c_l5 = NodeContainer (n3c, l5);
    NodeContainer connection_3c_l6 = NodeContainer (n3c, l6);
    
    NodeContainer connection_3d_l7 = NodeContainer (n3d, l7);
    NodeContainer connection_3d_l8 = NodeContainer (n3d, l8);
*/
       

   
 //   InternetStackHelper internet;
     
//    internet.Install (connection_1a_2a);
//    internet.Install (connection_1a_2b);
//    internet.Install (connection_1a_2c);
//
//    internet.Install (connection_1b_2b);
//    internet.Install (connection_1b_2c);
//    internet.Install (connection_1b_2d);
//









    

}
