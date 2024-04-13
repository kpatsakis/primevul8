afi_safi_print (afi_t afi, safi_t safi)
{
  if (afi == AFI_IP && safi == SAFI_UNICAST)
    return "IPv4 Unicast";
  else if (afi == AFI_IP && safi == SAFI_MULTICAST)
    return "IPv4 Multicast";
  else if (afi == AFI_IP && safi == SAFI_MPLS_VPN)
    return "VPNv4 Unicast";
  else if (afi == AFI_IP6 && safi == SAFI_UNICAST)
    return "IPv6 Unicast";
  else if (afi == AFI_IP6 && safi == SAFI_MULTICAST)
    return "IPv6 Multicast";
  else
    return "Unknown";
}