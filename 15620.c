DEFUN (neighbor_route_server_client,
       neighbor_route_server_client_cmd,
       NEIGHBOR_CMD2 "route-server-client",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Configure a neighbor as Route Server client\n")
{
  return peer_rsclient_set_vty (vty, argv[0], bgp_node_afi(vty),
                  bgp_node_safi(vty));
}