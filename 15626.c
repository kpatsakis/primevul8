DEFUN (neighbor_route_reflector_client,
       neighbor_route_reflector_client_cmd,
       NEIGHBOR_CMD2 "route-reflector-client",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Configure a neighbor as Route Reflector client\n")
{
  struct peer *peer;


  peer = peer_and_group_lookup_vty (vty, argv[0]);
  if (! peer)
    return CMD_WARNING;

  return peer_af_flag_set_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty),
			       PEER_FLAG_REFLECTOR_CLIENT);
}