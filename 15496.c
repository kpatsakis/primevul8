DEFUN (neighbor_port,
       neighbor_port_cmd,
       NEIGHBOR_CMD "port <0-65535>",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "Neighbor's BGP port\n"
       "TCP port number\n")
{
  return peer_port_vty (vty, argv[0], AFI_IP, argv[1]);
}