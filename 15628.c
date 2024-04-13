DEFUN (no_neighbor_distribute_list,
       no_neighbor_distribute_list_cmd,
       NO_NEIGHBOR_CMD2 "distribute-list (<1-199>|<1300-2699>|WORD) (in|out)",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Filter updates to/from this neighbor\n"
       "IP access-list number\n"
       "IP access-list number (expanded range)\n"
       "IP Access-list name\n"
       "Filter incoming updates\n"
       "Filter outgoing updates\n")
{
  return peer_distribute_unset_vty (vty, argv[0], bgp_node_afi (vty),
				    bgp_node_safi (vty), argv[2]);
}