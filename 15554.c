DEFUN (no_neighbor_route_server_client,
       no_neighbor_route_server_client_cmd,
       NO_NEIGHBOR_CMD2 "route-server-client",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Configure a neighbor as Route Server client\n")
{
  return peer_rsclient_unset_vty (vty, argv[0], bgp_node_afi(vty),
                  bgp_node_safi(vty));
}