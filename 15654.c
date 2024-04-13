DEFUN (no_neighbor_interface,
       no_neighbor_interface_cmd,
       NO_NEIGHBOR_CMD "interface WORD",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "Interface\n"
       "Interface name\n")
{
  return peer_interface_vty (vty, argv[0], NULL);
}