DEFUN (neighbor_remove_private_as,
       neighbor_remove_private_as_cmd,
       NEIGHBOR_CMD2 "remove-private-AS",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Remove private AS number from outbound updates\n")
{
  return peer_af_flag_set_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty),
			       PEER_FLAG_REMOVE_PRIVATE_AS);
}