DEFUN_DEPRECATED (neighbor_transparent_as,
		  neighbor_transparent_as_cmd,
		  NEIGHBOR_CMD "transparent-as",
		  NEIGHBOR_STR
		  NEIGHBOR_ADDR_STR
		  "Do not append my AS number even peer is EBGP peer\n")
{
  return peer_af_flag_set_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty),
			       PEER_FLAG_AS_PATH_UNCHANGED);
}