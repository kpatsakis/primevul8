DEFUN_DEPRECATED (neighbor_transparent_nexthop,
		  neighbor_transparent_nexthop_cmd,
		  NEIGHBOR_CMD "transparent-nexthop",
		  NEIGHBOR_STR
		  NEIGHBOR_ADDR_STR
		  "Do not change nexthop even peer is EBGP peer\n")
{
  return peer_af_flag_set_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty),
			       PEER_FLAG_NEXTHOP_UNCHANGED);
}