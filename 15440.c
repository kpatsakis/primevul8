DEFUN (no_neighbor_prefix_list,
       no_neighbor_prefix_list_cmd,
       NO_NEIGHBOR_CMD2 "prefix-list WORD (in|out)",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Filter updates to/from this neighbor\n"
       "Name of a prefix list\n"
       "Filter incoming updates\n"
       "Filter outgoing updates\n")
{
  return peer_prefix_list_unset_vty (vty, argv[0], bgp_node_afi (vty),
				     bgp_node_safi (vty), argv[2]);
}