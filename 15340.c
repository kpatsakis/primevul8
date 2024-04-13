DEFUN (no_neighbor_send_community_type,
       no_neighbor_send_community_type_cmd,
       NO_NEIGHBOR_CMD2 "send-community (both|extended|standard)",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Send Community attribute to this neighbor\n"
       "Send Standard and Extended Community attributes\n"
       "Send Extended Community attributes\n"
       "Send Standard Community attributes\n")
{
  if (strncmp (argv[1], "s", 1) == 0)
    return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
				   bgp_node_safi (vty),
				   PEER_FLAG_SEND_COMMUNITY);
  if (strncmp (argv[1], "e", 1) == 0)
    return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
				   bgp_node_safi (vty),
				   PEER_FLAG_SEND_EXT_COMMUNITY);

  return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
				 bgp_node_safi (vty),
				 (PEER_FLAG_SEND_COMMUNITY |
				  PEER_FLAG_SEND_EXT_COMMUNITY));
}