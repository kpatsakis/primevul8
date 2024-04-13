DEFUN (neighbor_shutdown,
       neighbor_shutdown_cmd,
       NEIGHBOR_CMD2 "shutdown",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Administratively shut down this neighbor\n")
{
  return peer_flag_set_vty (vty, argv[0], PEER_FLAG_SHUTDOWN);
}