DEFUN (neighbor_maximum_prefix_threshold_restart,
       neighbor_maximum_prefix_threshold_restart_cmd,
       NEIGHBOR_CMD2 "maximum-prefix <1-4294967295> <1-100> restart <1-65535>",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Maximum number of prefix accept from this peer\n"
       "maximum no. of prefix limit\n"
       "Threshold value (%) at which to generate a warning msg\n"
       "Restart bgp connection after limit is exceeded\n"
       "Restart interval in minutes")
{
  return peer_maximum_prefix_set_vty (vty, argv[0], bgp_node_afi (vty),
				      bgp_node_safi (vty), argv[1], argv[2], 0, argv[3]);
}