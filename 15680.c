DEFUN (neighbor_attr_unchanged,
       neighbor_attr_unchanged_cmd,
       NEIGHBOR_CMD2 "attribute-unchanged",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "BGP attribute is propagated unchanged to this neighbor\n")
{
  return peer_af_flag_set_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty),
			       (PEER_FLAG_AS_PATH_UNCHANGED |
				PEER_FLAG_NEXTHOP_UNCHANGED |
				PEER_FLAG_MED_UNCHANGED));
}