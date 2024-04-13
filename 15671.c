DEFUN (no_neighbor_nexthop_local_unchanged,
       no_neighbor_nexthop_local_unchanged_cmd,
       NO_NEIGHBOR_CMD2 "nexthop-local unchanged",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Configure treatment of outgoing link-local-nexthop attribute\n"
       "Leave link-local nexthop unchanged for this peer\n")
{
  return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
				 bgp_node_safi (vty),
                                PEER_FLAG_NEXTHOP_LOCAL_UNCHANGED );
}