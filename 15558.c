safi2str (safi_t safi)
{
  if (safi == SAFI_UNICAST)
    return "SAFI_UNICAST";
  else if (safi == SAFI_MULTICAST)
    return "SAFI_MULTICAST";
  else if (safi == SAFI_MPLS_VPN || safi == BGP_SAFI_VPNV4)
    return "SAFI_MPLS_VPN";
  else
    return "Unknown SAFI";
}