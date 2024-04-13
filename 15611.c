DEFUN (neighbor_filter_list,
       neighbor_filter_list_cmd,
       NEIGHBOR_CMD2 "filter-list WORD (in|out)",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Establish BGP filters\n"
       "AS path access-list name\n"
       "Filter incoming routes\n"
       "Filter outgoing routes\n")
{
  return peer_aslist_set_vty (vty, argv[0], bgp_node_afi (vty),
			      bgp_node_safi (vty), argv[1], argv[2]);
}