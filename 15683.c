DEFUN (neighbor_maximum_prefix_warning,
       neighbor_maximum_prefix_warning_cmd,
       NEIGHBOR_CMD2 "maximum-prefix <1-4294967295> warning-only",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Maximum number of prefix accept from this peer\n"
       "maximum no. of prefix limit\n"
       "Only give warning message when limit is exceeded\n")
{
  return peer_maximum_prefix_set_vty (vty, argv[0], bgp_node_afi (vty),
				      bgp_node_safi (vty), argv[1], NULL, 1,
				      NULL);
}