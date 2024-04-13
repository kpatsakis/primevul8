DEFUN (no_neighbor_route_map,
       no_neighbor_route_map_cmd,
       NO_NEIGHBOR_CMD2 "route-map WORD (in|out|import|export)",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Apply route map to neighbor\n"
       "Name of route map\n"
       "Apply map to incoming routes\n"
       "Apply map to outbound routes\n"
       "Apply map to routes going into a Route-Server client's table\n"
       "Apply map to routes coming from a Route-Server client")
{
  return peer_route_map_unset_vty (vty, argv[0], bgp_node_afi (vty),
				   bgp_node_safi (vty), argv[2]);
}