DEFUN (no_neighbor_maximum_prefix,
       no_neighbor_maximum_prefix_cmd,
       NO_NEIGHBOR_CMD2 "maximum-prefix",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Maximum number of prefix accept from this peer\n")
{
  return peer_maximum_prefix_unset_vty (vty, argv[0], bgp_node_afi (vty),
					bgp_node_safi (vty));
}