DEFUN (neighbor_send_community,
       neighbor_send_community_cmd,
       NEIGHBOR_CMD2 "send-community",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Send Community attribute to this neighbor\n")
{
  return peer_af_flag_set_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty),
			       PEER_FLAG_SEND_COMMUNITY);
}