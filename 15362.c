DEFUN (neighbor_unsuppress_map,
       neighbor_unsuppress_map_cmd,
       NEIGHBOR_CMD2 "unsuppress-map WORD",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Route-map to selectively unsuppress suppressed routes\n"
       "Name of route map\n")
{
  return peer_unsuppress_map_set_vty (vty, argv[0], bgp_node_afi (vty),
				      bgp_node_safi (vty), argv[1]);
}