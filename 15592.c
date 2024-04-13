DEFUN (neighbor_nexthop_self,
       neighbor_nexthop_self_cmd,
       NEIGHBOR_CMD2 "next-hop-self",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Disable the next hop calculation for this neighbor\n")
{
  return peer_af_flag_set_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty), PEER_FLAG_NEXTHOP_SELF);
}