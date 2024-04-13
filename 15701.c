DEFUN (neighbor_interface,
       neighbor_interface_cmd,
       NEIGHBOR_CMD "interface WORD",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "Interface\n"
       "Interface name\n")
{
  return peer_interface_vty (vty, argv[0], argv[1]);
}