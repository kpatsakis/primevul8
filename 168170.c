int main(int argc, char** argv)
try
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("version", "show version number")
    ("packet-limit", po::value<uint32_t>()->default_value(0), "stop after this many packets")
    ("quiet", po::value<bool>()->default_value(true), "don't be too noisy")
    ("recursive", po::value<bool>()->default_value(true), "look at recursion desired packets, or not (defaults true)")
    ("speedup", po::value<float>()->default_value(1), "replay at this speedup")
    ("timeout-msec", po::value<uint32_t>()->default_value(500), "wait at least this many milliseconds for a reply")
    ("ecs-stamp", "Add original IP address to ECS in replay")
    ("ecs-mask", po::value<uint16_t>(), "Replace first octet of src IP address with this value in ECS")
    ("source-ip", po::value<string>()->default_value(""), "IP to send the replayed packet from")
    ("source-port", po::value<uint16_t>()->default_value(0), "Port to send the replayed packet from");

  po::options_description alloptions;
  po::options_description hidden("hidden options");
  hidden.add_options()
    ("pcap-source", po::value<string>(), "PCAP source file")
    ("target-ip", po::value<string>()->default_value("127.0.0.1"), "target-ip")
    ("target-port", po::value<uint16_t>()->default_value(5300), "target port");

  alloptions.add(desc).add(hidden);
  po::positional_options_description p;
  p.add("pcap-source", 1);
  p.add("target-ip", 1);
  p.add("target-port", 1);

  po::store(po::command_line_parser(argc, argv).options(alloptions).positional(p).run(), g_vm);
  po::notify(g_vm);

  reportAllTypes();

  if (g_vm.count("help")) {
    usage(desc);
    return EXIT_SUCCESS;
  }

  if (g_vm.count("version")) {
    cerr<<"dnsreplay "<<VERSION<<endl;
    return EXIT_SUCCESS;
  }

  if(!g_vm.count("pcap-source")) {
    cerr<<"Fatal, need to specify at least a PCAP source file"<<endl;
    usage(desc);
    return EXIT_FAILURE;
  }

  uint32_t packetLimit = g_vm["packet-limit"].as<uint32_t>();

  g_rdSelector = g_vm["recursive"].as<bool>();

  g_quiet = g_vm["quiet"].as<bool>();

  signal(SIGINT, pleaseQuitHandler);
  float speedup=g_vm["speedup"].as<float>();
  g_timeoutMsec=g_vm["timeout-msec"].as<uint32_t>();

  PcapPacketReader pr(g_vm["pcap-source"].as<string>());
  s_socket= new Socket(AF_INET, SOCK_DGRAM);

  s_socket->setNonBlocking();

  if(g_vm.count("source-ip") && !g_vm["source-ip"].as<string>().empty())
    s_socket->bind(ComboAddress(g_vm["source-ip"].as<string>(), g_vm["source-port"].as<uint16_t>()));

  setSocketReceiveBuffer(s_socket->getHandle(), 2000000);
  setSocketSendBuffer(s_socket->getHandle(), 2000000);

  ComboAddress remote(g_vm["target-ip"].as<string>(), 
                    g_vm["target-port"].as<uint16_t>());

 int stamp = -1;
 if(g_vm.count("ecs-stamp") && g_vm.count("ecs-mask"))
   stamp=g_vm["ecs-mask"].as<uint16_t>();

  cerr<<"Replaying packets to: '"<<g_vm["target-ip"].as<string>()<<"', port "<<g_vm["target-port"].as<uint16_t>()<<endl;

  unsigned int once=0;
  struct timeval mental_time;
  mental_time.tv_sec=0; mental_time.tv_usec=0;

  if(!pr.getUDPPacket()) // we do this here so we error out more cleanly on no packets
    return 0;
  unsigned int count=0;
  bool first = true;
  for(;;) {
    if(g_pleaseQuit) {
      cerr<<"Interrupted from terminal"<<endl;
      break;
    }
    if(!((once++)%100)) 
      houseKeeping();
    
    struct timeval packet_ts;
    packet_ts.tv_sec = 0; 
    packet_ts.tv_usec = 0; 

    while(packet_ts < mental_time) {
      if(!first && !pr.getUDPPacket()) // otherwise we miss the first packet
        goto out;
      first=false;

      packet_ts.tv_sec = pr.d_pheader.ts.tv_sec;
      packet_ts.tv_usec = pr.d_pheader.ts.tv_usec;

      if(sendPacketFromPR(pr, remote, stamp))
        count++;
    } 
    if(packetLimit && count >= packetLimit) 
      break;

    mental_time=packet_ts;
    struct timeval then, now;
    gettimeofday(&then,0);

    receiveFromReference();

    gettimeofday(&now, 0);

    mental_time= mental_time + speedup * (now-then);
  }
 out:;
  sleep(1);
  receiveFromReference();
  printStats();
  emitFlightTimes();
}