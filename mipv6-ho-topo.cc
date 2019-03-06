/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Jadavpur University, India
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
 * Author: Manoj Kumar Rana <manoj24.rana@gmail.com>
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/csma-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/bridge-module.h"
#include "ns3/ipv6-static-routing.h"
#include "ns3/ipv6-list-routing-helper.h"
#include "ns3/ipv6-routing-table-entry.h"
#include "ns3/mipv6-module.h"
#include "ns3/internet-trace-helper.h"
#include "ns3/trace-helper.h"
#include "ns3/internet-apps-module.h"
#include "ns3/radvd.h"
#include "ns3/radvd-interface.h"
#include "ns3/radvd-prefix.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>


using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("mip6Wifi");


int main (int argc, char *argv[])
{

NodeContainer sta;
NodeContainer ha;
NodeContainer cn;
NodeContainer ars;
NodeContainer mid;
NodeContainer backbone1;
NodeContainer backbone2;
NodeContainer backbone3;

CommandLine cmd;
cmd.Parse (argc, argv);

ars.Create (2);
ha.Create (1);
sta.Create (1);
cn.Create (1);
mid.Create (3);



NetDeviceContainer staDevs;
NetDeviceContainer haDevs;
NetDeviceContainer cnDevs;
NetDeviceContainer ar1Devs;
NetDeviceContainer ar2Devs;
NetDeviceContainer midDevs;
NetDeviceContainer backbone1Devs;
NetDeviceContainer backbone2Devs;
NetDeviceContainer backbone3Devs;



Ipv6InterfaceContainer staIfs;
Ipv6InterfaceContainer haIfs;
Ipv6InterfaceContainer cnIfs;
Ipv6InterfaceContainer ar1Ifs;
Ipv6InterfaceContainer ar2Ifs;
Ipv6InterfaceContainer midIfs;
Ipv6InterfaceContainer backbone1Ifs;
Ipv6InterfaceContainer backbone2Ifs;
Ipv6InterfaceContainer backbone3Ifs;



InternetStackHelper internet;

internet.Install (ars);
internet.Install (mid);
internet.Install (ha);
internet.Install (cn);
internet.Install (sta);

// Adding channel into nodes
backbone2.Add (mid.Get (1));
backbone2.Add (mid.Get (2));
backbone2.Add (ars.Get (0));
backbone2.Add (ars.Get (1));

backbone1.Add (mid.Get (0));
backbone1.Add (mid.Get (1));
backbone1.Add (mid.Get (2));

backbone3.Add (cn.Get (0));
backbone3.Add (mid.Get (0));
backbone3.Add (ha.Get (0));

// Making Channel Container
CsmaHelper csma;
Ipv6AddressHelper ipv6;
Ipv6InterfaceContainer iifc;

// Adding Channel into backbone
csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate ("50Mbps")));
csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (1)));
csma.SetDeviceAttribute ("Mtu", UintegerValue (1400));
backbone2Devs = csma.Install (backbone2);
backbone1Devs = csma.Install (backbone1);
backbone3Devs = csma.Install (backbone3);


ipv6.SetBase (Ipv6Address ("2001:db80::"), Ipv6Prefix (64));
iifc = ipv6.Assign (backbone2Devs);
backbone2Ifs.Add (iifc);
ipv6.SetBase (Ipv6Address ("3001:db80::"), Ipv6Prefix (64));
iifc = ipv6.Assign (backbone1Devs);
backbone1Ifs.Add (iifc); 
ipv6.SetBase (Ipv6Address ("5001:db80::"), Ipv6Prefix (64));
iifc = ipv6.Assign (backbone3Devs);
backbone3Ifs.Add (iifc);


backbone2Ifs.SetForwarding (0,true); // 2nd backbone
backbone2Ifs.SetForwarding (1,true);
backbone2Ifs.SetForwarding (2,true);
backbone1Ifs.SetForwarding (0,true); // 1st backbone
backbone1Ifs.SetForwarding (1,true);
backbone1Ifs.SetForwarding (2,true);
backbone3Ifs.SetForwarding (0,true); // 3th backbone
backbone3Ifs.SetForwarding (1,true);
backbone3Ifs.SetForwarding (2,true);
backbone2Ifs.SetDefaultRouteInAllNodes (0); // 2nd backbone
backbone2Ifs.SetDefaultRouteInAllNodes (1);
backbone2Ifs.SetDefaultRouteInAllNodes (2);
backbone1Ifs.SetDefaultRouteInAllNodes (0); // 1st backbone
backbone1Ifs.SetDefaultRouteInAllNodes (1);
backbone1Ifs.SetDefaultRouteInAllNodes (2);
backbone3Ifs.SetDefaultRouteInAllNodes (0); // 3th backbone
backbone3Ifs.SetDefaultRouteInAllNodes (1);
backbone3Ifs.SetDefaultRouteInAllNodes (2);

// Static mobility for router, AP, and others
MobilityHelper mobility;
Ptr<ListPositionAllocator> positionAlloc;
positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (0.0, -40.0, 0.0));  //MID
positionAlloc->Add (Vector (-50.0, 40.0, 0.0)); //AR1
positionAlloc->Add (Vector (50.0, 40.0, 0.0));  //AR2

mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (backbone2);

positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (25.0, -40.0, 0.0));
mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (ha);

positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (-25.0, -40.0, 0.0));
mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (cn);


Ssid ssid = Ssid ("ns-3-ssid");
YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

WifiHelper wifi;
NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
wifiPhy.SetChannel (wifiChannel.Create ());
   
wifiMac.SetType ("ns3::ApWifiMac",
		           "Ssid", SsidValue (ssid),
		           "BeaconGeneration", BooleanValue (true),
		           "BeaconInterval", TimeValue (MilliSeconds(100)));

ar1Devs = wifi.Install (wifiPhy, wifiMac, ars.Get(0));
ar2Devs = wifi.Install (wifiPhy, wifiMac, ars.Get(1));

Ipv6AddressHelper ipv62;
ipv62.SetBase (Ipv6Address ("8888:56ac::"), Ipv6Prefix (64));
iifc = ipv62.Assign (ar1Devs);
ar1Ifs.Add (iifc);
ar1Ifs.SetForwarding (0, true);
ar1Ifs.SetDefaultRouteInAllNodes (0);
  

Ipv6AddressHelper ipv63;
ipv63.SetBase (Ipv6Address ("9999:db80::"), Ipv6Prefix (64));
iifc = ipv63.Assign (ar2Devs);
ar2Ifs.Add (iifc);
ar2Ifs.SetForwarding (0, true);
ar2Ifs.SetDefaultRouteInAllNodes (0);


positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (-50.0, 70.0, 0.0)); //STA
mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");  
mobility.Install (sta);

Ptr<ConstantVelocityMobilityModel> cvm = sta.Get (0)->GetObject<ConstantVelocityMobilityModel> ();
cvm->SetVelocity (Vector (5, 0, 0)); //move left to right


wifiMac.SetType ("ns3::StaWifiMac",
	               "Ssid", SsidValue (ssid),
	               "ActiveProbing", BooleanValue (false));
staDevs.Add ( wifi.Install (wifiPhy, wifiMac, sta));
iifc = ipv6.AssignWithoutAddress (staDevs);
staIfs.Add (iifc);




Ipv6Address prefix ("8888:56ac::");  //create the prefix 
uint32_t indexRouter = ar1Ifs.GetInterfaceIndex (0);  //AR interface (n-AR1) 

Ptr<Radvd> radvd=CreateObject<Radvd> ();
Ptr<RadvdInterface> routerInterface= Create<RadvdInterface> (indexRouter, 1500, 50);
Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 1.5, 2.0);

routerInterface->AddPrefix (routerPrefix);

radvd->AddConfiguration (routerInterface);

ars.Get(0)->AddApplication (radvd);
radvd->SetStartTime(Seconds (1.0));
radvd->SetStopTime(Seconds (100.0));



Ipv6Address prefix2 ("9999:db80::");  //create the prefix 
uint32_t indexRouter2 = ar2Ifs.GetInterfaceIndex (0);  //AR interface (R-n1) 

Ptr<Radvd> radvd2=CreateObject<Radvd> ();
Ptr<RadvdInterface> routerInterface2 = Create<RadvdInterface> (indexRouter2, 1500, 50);
Ptr<RadvdPrefix> routerPrefix2 = Create<RadvdPrefix> (prefix2, 64, 1.5, 2.0);

routerInterface2->AddPrefix (routerPrefix2);

radvd2->AddConfiguration (routerInterface2);

ars.Get (1)->AddApplication (radvd2);
radvd2->SetStartTime(Seconds (4.1));
radvd2->SetStopTime(Seconds (100.0));

/* ----------    origin    -----------
Ipv6StaticRoutingHelper routingHelper;
Ptr<Ipv6> ipv692 = mid.Get(0)->GetObject<Ipv6> ();
Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("8888:56ac::"), Ipv6Prefix (64), Ipv6Address ("2001:db80::200:ff:fe00:2"), 1, 0);
rttop->AddNetworkRouteTo (Ipv6Address ("9999:db80::"), Ipv6Prefix (64), Ipv6Address ("2001:db80::200:ff:fe00:3"), 1, 0);
ipv692 = ars.Get(0)->GetObject<Ipv6> ();
rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("5001:db80::"), Ipv6Prefix(64), Ipv6Address("2001:db80::200:ff:fe00:1"), 1, 0);
ipv692 = ars.Get(1)->GetObject<Ipv6> ();
rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("5001:db80::"), Ipv6Prefix (64), Ipv6Address("2001:db80::200:ff:fe00:1"), 1, 0);

ipv692 = cn.Get(0)->GetObject<Ipv6> ();
rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("8888:56ac::"), Ipv6Prefix (64), Ipv6Address ("5001:db80::200:ff:fe00:5"), 1, 0);
rttop->AddNetworkRouteTo (Ipv6Address ("9999:db80::"), Ipv6Prefix (64), Ipv6Address ("5001:db80::200:ff:fe00:5"), 1, 0);
 ----------    end of origin    ----------- */


// -----------    route     ----------
Ipv6StaticRoutingHelper routingHelper;

Ptr<Ipv6> ipv692 = mid.Get(0)->GetObject<Ipv6> ();
Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("8888:56ac::"), Ipv6Prefix (64), Ipv6Address ("3001:db80::200:ff:fe00:1"), 1, 0);
rttop->AddNetworkRouteTo (Ipv6Address ("9999:db80::"), Ipv6Prefix (64), Ipv6Address ("3001:db80::200:ff:fe00:2"), 1, 0);

Ptr<Ipv6> ipv692 = mid.Get(1)->GetObject<Ipv6> ();
Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("8888:56ac::"), Ipv6Prefix (64), Ipv6Address ("2001:db80::200:ff:fe00:1"), 1, 0);

Ptr<Ipv6> ipv692 = mid.Get(2)->GetObject<Ipv6> ();
Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("9999:db80::"), Ipv6Prefix (64), Ipv6Address ("2001:db80::200:ff:fe00:1"), 1, 0);

ipv692 = ars.Get(0)->GetObject<Ipv6> ();
rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("5001:db80::"), Ipv6Prefix(64), Ipv6Address("2001:db80::200:ff:fe00:1"), 1, 0);

ipv692 = ars.Get(1)->GetObject<Ipv6> ();
rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("5001:db80::"), Ipv6Prefix (64), Ipv6Address("2001:db80::200:ff:fe00:1"), 1, 0);

ipv692 = cn.Get(0)->GetObject<Ipv6> ();
rttop = routingHelper.GetStaticRouting (ipv692);
rttop->AddNetworkRouteTo (Ipv6Address ("8888:56ac::"), Ipv6Prefix (64), Ipv6Address ("5001:db80::200:ff:fe00:5"), 1, 0);
rttop->AddNetworkRouteTo (Ipv6Address ("9999:db80::"), Ipv6Prefix (64), Ipv6Address ("5001:db80::200:ff:fe00:5"), 1, 0);
// ----------- end of route ----------

//Installing MIPv6
Mipv6HaHelper hahelper;
hahelper.Install (ha.Get (0));
Mipv6MnHelper mnhelper (hahelper.GetHomeAgentAddressList (),false); 
mnhelper.Install (sta.Get (0));

//APP
UdpEchoServerHelper echoServer (9);

ApplicationContainer serverApps = echoServer.Install (cn.Get (0));

serverApps.Start (Seconds (1.0));
serverApps.Stop (Seconds (700.0));

UdpEchoClientHelper echoClient (Ipv6Address ("5001:db80::200:ff:fe00:4"), 9);
echoClient.SetAttribute ("MaxPackets", UintegerValue (10000));
echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.05)));
echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

ApplicationContainer clientApps = echoClient.Install (sta.Get (0));

clientApps.Start (Seconds (1.1));
clientApps.Stop (Seconds (700.0));  


LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_ALL);
LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_ALL);

Simulator::Stop (Seconds (100.0));
Simulator::Run ();
Simulator::Destroy ();

return 0;
}
