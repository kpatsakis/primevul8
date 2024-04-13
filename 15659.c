DEFUN (no_neighbor_soft_reconfiguration,
       no_neighbor_soft_reconfiguration_cmd,
       NO_NEIGHBOR_CMD2 "soft-reconfiguration inbound",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Per neighbor soft reconfiguration\n"
       "Allow inbound soft reconfiguration for this neighbor\n")
{
  return peer_af_flag_unset_vty (vty, argv[0],
				 bgp_node_afi (vty), bgp_node_safi (vty),
				 PEER_FLAG_SOFT_RECONFIG);
}