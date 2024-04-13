DEFUN (no_neighbor_attr_unchanged,
       no_neighbor_attr_unchanged_cmd,
       NO_NEIGHBOR_CMD2 "attribute-unchanged",
       NO_STR	 
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "BGP attribute is propagated unchanged to this neighbor\n")
{
  return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
				 bgp_node_safi (vty),
				 (PEER_FLAG_AS_PATH_UNCHANGED |
				  PEER_FLAG_NEXTHOP_UNCHANGED |
				  PEER_FLAG_MED_UNCHANGED));
}