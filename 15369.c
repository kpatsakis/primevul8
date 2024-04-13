bgp_node_safi (struct vty *vty)
{
  if (vty->node == BGP_VPNV4_NODE)
    return SAFI_MPLS_VPN;
  if (vty->node == BGP_IPV4M_NODE || vty->node == BGP_IPV6M_NODE)
    return SAFI_MULTICAST;
  return SAFI_UNICAST;
}