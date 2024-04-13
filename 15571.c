DEFUN (no_neighbor_send_community,
       no_neighbor_send_community_cmd,
       NO_NEIGHBOR_CMD2 "send-community",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Send Community attribute to this neighbor\n")
{
  return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
				 bgp_node_safi (vty),
				 PEER_FLAG_SEND_COMMUNITY);
}