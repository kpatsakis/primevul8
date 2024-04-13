DEFUN (no_neighbor_filter_list,
       no_neighbor_filter_list_cmd,
       NO_NEIGHBOR_CMD2 "filter-list WORD (in|out)",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Establish BGP filters\n"
       "AS path access-list name\n"
       "Filter incoming routes\n"
       "Filter outgoing routes\n")
{
  return peer_aslist_unset_vty (vty, argv[0], bgp_node_afi (vty),
				bgp_node_safi (vty), argv[2]);
}