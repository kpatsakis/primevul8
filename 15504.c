DEFUN (neighbor_soft_reconfiguration,
       neighbor_soft_reconfiguration_cmd,
       NEIGHBOR_CMD2 "soft-reconfiguration inbound",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Per neighbor soft reconfiguration\n"
       "Allow inbound soft reconfiguration for this neighbor\n")
{
  return peer_af_flag_set_vty (vty, argv[0],
			       bgp_node_afi (vty), bgp_node_safi (vty),
			       PEER_FLAG_SOFT_RECONFIG);
}