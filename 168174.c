  DNSIDManager()
  {
    for(unsigned int i=0; i < 65536; ++i)
      d_available.push_back(i);
  }