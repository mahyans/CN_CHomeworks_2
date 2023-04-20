#include <cstdlib>
#include<time.h>
#include <stdio.h>
#include <string>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/error-model.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("WifiTopology");

void
ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, double em)
{
    uint16_t i = 0;

    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats ();

    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);

        std::cout << "Flow ID			: "<< stats->first << " ; " << fiveTuple.sourceAddress << " -----> " << fiveTuple.destinationAddress << std::endl;
        std::cout << "Tx Packets = " << stats->second.txPackets << std::endl;
        std::cout << "Rx Packets = " << stats->second.rxPackets << std::endl;
        std::cout << "Duration		: "<< (stats->second.timeLastRxPacket.GetSeconds () - stats->second.timeFirstTxPacket.GetSeconds ()) << std::endl;
        std::cout << "Last Received Packet	: "<< stats->second.timeLastRxPacket.GetSeconds () << " Seconds" << std::endl;
        std::cout << "Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds () - stats->second.timeFirstTxPacket.GetSeconds ()) / 1024 / 1024  << " Mbps" << std::endl;
    
        i++;

        std::cout << "---------------------------------------------------------------------------" << std::endl;
    }

    Simulator::Schedule (Seconds (10),&ThroughputMonitor, fmhelper, flowMon, em);
}

void
AverageDelayMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, double em)
{
    uint16_t i = 0;

    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats ();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
        std::cout << "Flow ID			: "<< stats->first << " ; " << fiveTuple.sourceAddress << " -----> " << fiveTuple.destinationAddress << std::endl;
        std::cout << "Tx Packets = " << stats->second.txPackets << std::endl;
        std::cout << "Rx Packets = " << stats->second.rxPackets << std::endl;
        std::cout << "Duration		: "<< (stats->second.timeLastRxPacket.GetSeconds () - stats->second.timeFirstTxPacket.GetSeconds ()) << std::endl;
        std::cout << "Last Received Packet	: "<< stats->second.timeLastRxPacket.GetSeconds () << " Seconds" << std::endl;
        std::cout << "Sum of e2e Delay: " << stats->second.delaySum.GetSeconds () << " s" << std::endl;
        std::cout << "Average of e2e Delay: " << stats->second.delaySum.GetSeconds () / stats->second.rxPackets << " s" << std::endl;
    
        i++;

        std::cout << "---------------------------------------------------------------------------" << std::endl;
    }

    Simulator::Schedule (Seconds (10),&AverageDelayMonitor, fmhelper, flowMon, em);
}

class MyHeader : public Header 
{
public:
    MyHeader ();
    virtual ~MyHeader ();
    void SetData (uint16_t data);
    void SetData_str(string s_data);
    uint16_t GetData (void) const;
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;
    virtual void Print (std::ostream &os) const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);
    virtual uint32_t GetSerializedSize (void) const;
private:
    uint16_t m_data;
    string s_data;
};

MyHeader::MyHeader ()
{
}

MyHeader::~MyHeader ()
{
}

TypeId
MyHeader::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::MyHeader")
        .SetParent<Header> ()
        .AddConstructor<MyHeader> ()
    ;
    return tid;
}

TypeId
MyHeader::GetInstanceTypeId (void) const
{
    return GetTypeId ();
}

void
MyHeader::Print (std::ostream &os) const
{
    os << "m_data = " << m_data << endl;
}

uint32_t
MyHeader::GetSerializedSize (void) const
{
    return 2;
}

void
MyHeader::Serialize (Buffer::Iterator start) const
{
    start.WriteHtonU16 (m_data);
}

uint32_t
MyHeader::Deserialize (Buffer::Iterator start)
{
    m_data = start.ReadNtohU16 ();

    return 2;
}

void 
MyHeader::SetData (uint16_t data)
{
    m_data = data;
}

uint16_t 
MyHeader::GetData (void) const
{
    return m_data;
}


class StrHeader : public Header 
{
public:
    StrHeader ();
    virtual ~StrHeader ();
    void SetData (char data);
    char GetData (void) const;
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;
    virtual void Print (std::ostream &os) const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);
    virtual uint32_t GetSerializedSize (void) const;
private:
    char m_data;
};

StrHeader::StrHeader ()
{
}

StrHeader::~StrHeader ()
{
}

TypeId
StrHeader::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::StrHeader")
        .SetParent<Header> ()
        .AddConstructor<StrHeader> ()
    ;
    return tid;
}

TypeId
StrHeader::GetInstanceTypeId (void) const
{
    return GetTypeId ();
}

void
StrHeader::Print (std::ostream &os) const
{
    os << "char:  " << m_data << endl;
}

uint32_t
StrHeader::GetSerializedSize (void) const
{
    return 2;
}

void
StrHeader::Serialize (Buffer::Iterator start) const
{
    start.WriteHtonU16 (m_data);
}

uint32_t
StrHeader::Deserialize (Buffer::Iterator start)
{
    m_data = start.ReadNtohU16 ();

    return 2;
}

void 
StrHeader::SetData (char data)
{
    m_data = data;
}

char 
StrHeader::GetData (void) const
{
    return m_data;
}


class master : public Application
{
public:
    master (uint16_t port,uint16_t tcp_port, Ipv4InterfaceContainer& ip, Ipv4InterfaceContainer& ip1, Ipv4InterfaceContainer& ip2, Ipv4InterfaceContainer& ip3);
    virtual ~master ();
private:
    virtual void StartApplication (void);
    void HandleRead (Ptr<Socket> socket);
    void sendToMapper(uint32_t s_data, Ptr<Socket> sock);

    uint16_t port;
    uint16_t tcp_port;
    Ipv4InterfaceContainer ip;
    Ipv4InterfaceContainer& ip1;
    Ipv4InterfaceContainer& ip2;
    Ipv4InterfaceContainer& ip3;
    Ptr<Socket> socket;
};


class client : public Application
{
public:
    client (uint16_t port, Ipv4InterfaceContainer& ip, Ipv4InterfaceContainer& ip2);
    virtual ~client ();

private:
    virtual void StartApplication (void);
    void HandleRead (Ptr<Socket> socket);
    
    uint16_t port;
    Ptr<Socket> socket;
    Ipv4InterfaceContainer ip;
    Ipv4InterfaceContainer ip2;
};

class mapper : public Application
{
public:
    mapper (map<int,char> dictionary, uint16_t port, uint16_t tcp_port, Ipv4InterfaceContainer& ip, Ipv4InterfaceContainer& ip2);
    virtual ~mapper ();

private:
    virtual void StartApplication (void);
    void HandleRead (Ptr<Socket> socket);
    void sendToClient(char s_data, Ptr<Socket> sock);

    map<int,char> dictionary;
    uint16_t port;
    uint16_t tcp_port;
    Ptr<Socket> socket;
    Ipv4InterfaceContainer ip;
    Ipv4InterfaceContainer ip2;
};



int
main (int argc, char *argv[])
{
    double error = 0.000001;
    string bandwidth = "1Mbps";
    bool verbose = true;
    double duration = 10.0;
    bool tracing = false;

    srand(time(NULL));
/*
    map<int,char> map1 { { 0, 100}, { 1, 101}, { 2, 102 }, { 3, 103}, { 4, 104 }, { 5, 105}, { 6, 106 },  { 7, 107}, { 8, 108 } };
    map<int,char> map2 { { 9, 209}, { 10, 210}, { 11, 211 }, { 12, 212}, { 13, 213 }, { 14, 214}, { 15, 215 },  { 16, 216}, { 17, 217 } };
    map<int,char> map3 { { 18, 318},{ 19, 319},{ 20, 320}, { 21, 321 }, { 22, 322}, { 23, 323 }, { 24, 324}, { 25, 325 },  { 26, 326} };
*/
    map<int, char> map1;
    map<int, char> map2;
    map<int, char> map3;

    map1[0] = 'a';
    map1[1] = 'b';
    map1[2] = 'c';
    map1[3] = 'd';
    map1[4] = 'e';
    map1[5] = 'f';
    map1[6] = 'g';
    map1[7] = 'h';
    map1[8] = 'i';

    map2[9] =  'j';
    map2[10] = 'k';
    map2[11] = 'l';
    map2[12] = 'm';
    map2[13] = 'n';
    map2[14] = 'o';
    map2[15] = 'p';
    map2[16] = 'q';
    map2[17] = 'r';
    
    map3[18] = 's';
    map3[19] = 't';
    map3[20] = 'u';
    map3[21] = 'v';
    map3[22] = 'w';
    map3[23] = 'x';
    map3[24] = 'y';
    map3[25] = 'z';

    CommandLine cmd (__FILE__);
    cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

    cmd.Parse (argc,argv);

    if (verbose)
    {
        LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    NodeContainer wifiStaNodeClient;
    wifiStaNodeClient.Create (1);

    NodeContainer wifiStaNodeMaster;
    wifiStaNodeMaster.Create (1);

    NodeContainer wifiStaNodeMapper1;
    wifiStaNodeMapper1.Create (1);

    NodeContainer wifiStaNodeMapper2;
    wifiStaNodeMapper2.Create (1);

    NodeContainer wifiStaNodeMapper3;
    wifiStaNodeMapper3.Create (1);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();

    YansWifiPhyHelper phy;
    phy.SetChannel (channel.Create ());

    WifiHelper wifi;
    wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

    WifiMacHelper mac;
    Ssid ssid = Ssid ("ns-3-ssid");
    mac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid),"ActiveProbing", BooleanValue (false));
    NetDeviceContainer staDeviceClient;
    staDeviceClient = wifi.Install (phy, mac, wifiStaNodeClient);

    mac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));
    NetDeviceContainer staDeviceMaster;
    staDeviceMaster = wifi.Install (phy, mac, wifiStaNodeMaster);

    mac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue (false));    
    NetDeviceContainer staDeviceMapper1;
    staDeviceMapper1 = wifi.Install (phy, mac, wifiStaNodeMapper1);

    mac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue (false));    
    NetDeviceContainer staDeviceMapper2;
    staDeviceMapper2 = wifi.Install (phy, mac, wifiStaNodeMapper2);

    mac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue (false));    
    NetDeviceContainer staDeviceMapper3;
    staDeviceMapper3 = wifi.Install (phy, mac, wifiStaNodeMapper3);


    

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
    em->SetAttribute ("ErrorRate", DoubleValue (error));
    phy.SetErrorRateModel("ns3::YansErrorRateModel");

    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                   "MinX", DoubleValue (0.0),
                                   "MinY", DoubleValue (0.0),
                                   "DeltaX", DoubleValue (5.0),
                                   "DeltaY", DoubleValue (10.0),
                                   "GridWidth", UintegerValue (3),
                                   "LayoutType", StringValue ("RowFirst"));

    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                               "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
    mobility.Install (wifiStaNodeClient);

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiStaNodeMaster);

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiStaNodeMapper1);

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiStaNodeMapper2);

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiStaNodeMapper3);

    InternetStackHelper stack;
    stack.Install (wifiStaNodeClient);
    stack.Install (wifiStaNodeMaster);
    stack.Install (wifiStaNodeMapper1);
    stack.Install (wifiStaNodeMapper2);
    stack.Install (wifiStaNodeMapper3);


    Ipv4AddressHelper address;

    Ipv4InterfaceContainer staNodeClientInterface;
    Ipv4InterfaceContainer staNodesMasterInterface;
    Ipv4InterfaceContainer staNodesMapper1Interface;
    Ipv4InterfaceContainer staNodesMapper2Interface;
    Ipv4InterfaceContainer staNodesMapper3Interface;



    address.SetBase ("10.1.3.0", "255.255.255.0");
    staNodeClientInterface = address.Assign (staDeviceClient);
    staNodesMasterInterface = address.Assign (staDeviceMaster);
    staNodesMapper1Interface = address.Assign (staDeviceMapper1);
    staNodesMapper2Interface = address.Assign (staDeviceMapper2);
    staNodesMapper3Interface = address.Assign (staDeviceMapper3);


    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    uint16_t port = 1102;
    uint16_t tcp_port = 1103;


    Ptr<client> clientApp = CreateObject<client> (port, staNodesMasterInterface, staNodeClientInterface);
    wifiStaNodeClient.Get (0)->AddApplication (clientApp);
    clientApp->SetStartTime (Seconds (0.0));
    clientApp->SetStopTime (Seconds (duration));  

    Ptr<master> masterApp = CreateObject<master> (port, tcp_port, staNodesMasterInterface, staNodesMapper1Interface, staNodesMapper2Interface, staNodesMapper3Interface);
    wifiStaNodeMaster.Get (0)->AddApplication (masterApp);
    masterApp->SetStartTime (Seconds (0.0));
    masterApp->SetStopTime (Seconds (duration));  

    Ptr<mapper> mapper1App = CreateObject<mapper> (map1, port, tcp_port, staNodesMapper1Interface, staNodeClientInterface);
    wifiStaNodeMapper1.Get (0)->AddApplication (mapper1App);
    mapper1App->SetStartTime (Seconds (0.0));
    mapper1App->SetStopTime (Seconds (duration)); 

    Ptr<mapper> mapper2App = CreateObject<mapper> (map2, port, tcp_port, staNodesMapper2Interface, staNodeClientInterface);
    wifiStaNodeMapper2.Get (0)->AddApplication (mapper2App);
    mapper2App->SetStartTime (Seconds (0.0));
    mapper2App->SetStopTime (Seconds (duration));

    Ptr<mapper> mapper3App = CreateObject<mapper> (map3, port, tcp_port, staNodesMapper3Interface, staNodeClientInterface);
    wifiStaNodeMapper3.Get (0)->AddApplication (mapper3App);
    mapper3App->SetStartTime (Seconds (0.0));
    mapper3App->SetStopTime (Seconds (duration));

    NS_LOG_INFO ("Run Simulation");

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    ThroughputMonitor (&flowHelper, flowMonitor, error);
    AverageDelayMonitor (&flowHelper, flowMonitor, error);

    Simulator::Stop (Seconds (duration));
    Simulator::Run ();

    return 0;
}

client::client (uint16_t port, Ipv4InterfaceContainer& ip, Ipv4InterfaceContainer& ip2)
        : port (port),
          ip (ip),
          ip2 (ip2)
{
    std::srand (time(0));
}

void 
client::HandleRead(Ptr<Socket> socket)
{
    Ptr<Packet> packet;

    while ((packet = socket->Recv ()))
    {
        if (packet->GetSize () == 0)
        {
            break;
        }

        StrHeader destinationHeader;
        packet->RemoveHeader (destinationHeader);
        destinationHeader.Print(std::cout);
    }
}

client::~client ()
{
}

static void GenerateTraffic (Ptr<Socket> socket, uint16_t data)
{
    Ptr<Packet> packet = new Packet();
    MyHeader m;
    m.SetData(data);

    packet->AddHeader (m);
    packet->Print (std::cout);
    socket->Send(packet);

    Simulator::Schedule (Seconds (0.1), &GenerateTraffic, socket, rand() % 27);
}

void
client::StartApplication (void)
{
    Ptr<Socket> sock = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress sockAddr (ip.GetAddress(0), port);
    sock->Connect (sockAddr);

    GenerateTraffic(sock, 0);

    socket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress local = InetSocketAddress (ip2.GetAddress(0), port);
    socket->Bind (local);

    socket->SetRecvCallback (MakeCallback (&client::HandleRead, this));
}

master::master (uint16_t port,uint16_t tcp_port, Ipv4InterfaceContainer& ip, Ipv4InterfaceContainer& ip1, Ipv4InterfaceContainer& ip2, Ipv4InterfaceContainer& ip3)
        : port (port),
          tcp_port (tcp_port),
          ip (ip),
          ip1 (ip1),
          ip2 (ip2),
          ip3 (ip3)
{
    std::srand (time(0));
}

void 
master::sendToMapper(uint32_t s_data, Ptr<Socket> sock)
{
    Ptr<Packet> packet = new Packet();
    MyHeader m;
    m.SetData(s_data);

    packet->AddHeader (m);
    packet->Print (std::cout);
    sock->Send(packet);
}

master::~master ()
{
}

void
master::StartApplication (void)
{
    socket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress local = InetSocketAddress (ip.GetAddress(0), port);
    socket->Bind (local);

    socket->SetRecvCallback (MakeCallback (&master::HandleRead, this));
}

void 
master::HandleRead (Ptr<Socket> socket)
{
    Ptr<Packet> packet;

    Ptr<Socket> sock1 = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress sockAddr1 (ip1.GetAddress(0), tcp_port);
    sock1->Connect (sockAddr1);

    Ptr<Socket> sock2 = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress sockAddr2 (ip2.GetAddress(0), tcp_port);
    sock2->Connect (sockAddr2);

    Ptr<Socket> sock3 = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress sockAddr3 (ip3.GetAddress(0), tcp_port);
    sock3->Connect (sockAddr3);

    while ((packet = socket->Recv ()))
    {
        if (packet->GetSize () == 0)
        {
            break;
        }
        MyHeader destinationHeader;
        packet->RemoveHeader (destinationHeader);
        //destinationHeader.Print(std::cout);
        int val = destinationHeader.GetData();
        sendToMapper( val, sock1);
        sendToMapper( val, sock2);
        sendToMapper( val, sock3);
    }
}

mapper::mapper (map<int,char> m, uint16_t port, uint16_t tcp_port, Ipv4InterfaceContainer& ip, Ipv4InterfaceContainer& ip2)
        : dictionary (m), 
          port (port),
          tcp_port (tcp_port),
          ip (ip),
          ip2 (ip2)
{
    std::srand (time(0));
}

mapper::~mapper ()
{
}

void
mapper::StartApplication (void)
{
    socket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress local = InetSocketAddress (ip.GetAddress(0), tcp_port);
    socket->Bind (local);

    socket->SetRecvCallback (MakeCallback (&mapper::HandleRead, this));
}

void 
mapper::HandleRead (Ptr<Socket> socket)
{
    Ptr<Packet> packet;

    Ptr<Socket> sock = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress sockAddr (ip2.GetAddress(0), port);
    sock->Connect (sockAddr);

    while ((packet = socket->Recv ()))
    {
        if (packet->GetSize () == 0)
        {
            break;
        }

        MyHeader destinationHeader;
        packet->RemoveHeader (destinationHeader);
        
        uint16_t val = destinationHeader.GetData();
        
        if (dictionary.find(val) != dictionary.end()){
            sendToClient( dictionary[val], sock);
        }
    }
}

void
mapper::sendToClient(char s_data, Ptr<Socket> sock)
{

    Ptr<Packet> packet = new Packet();
    StrHeader m;
    m.SetData(s_data);

    packet->AddHeader (m);
    packet->Print (std::cout);
    sock->Send(packet);
}