DEFUN (no_neighbor_port,
       no_neighbor_port_cmd,
       NO_NEIGHBOR_CMD "port",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "Neighbor's BGP port\n")
{
  return peer_port_vty (vty, argv[0], AFI_IP, NULL);
}