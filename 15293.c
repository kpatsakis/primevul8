DEFUN (no_neighbor_route_reflector_client,
       no_neighbor_route_reflector_client_cmd,
       NO_NEIGHBOR_CMD2 "route-reflector-client",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Configure a neighbor as Route Reflector client\n")
{
  return peer_af_flag_unset_vty (vty, argv[0], bgp_node_afi (vty),
				 bgp_node_safi (vty),
				 PEER_FLAG_REFLECTOR_CLIENT);
}