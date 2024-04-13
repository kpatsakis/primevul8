DEFUN (no_neighbor_nexthop_self,
       no_neighbor_nexthop_self_cmd,
       NO_NEIGHBOR_CMD2 "next-hop-self",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Disable the next hop calculation for this neighbor\n")
{
  return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
				 bgp_node_safi (vty), PEER_FLAG_NEXTHOP_SELF);
}