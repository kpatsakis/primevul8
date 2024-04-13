bgp_node_afi (struct vty *vty)
{
  if (vty->node == BGP_IPV6_NODE || vty->node == BGP_IPV6M_NODE)
    return AFI_IP6;
  return AFI_IP;
}