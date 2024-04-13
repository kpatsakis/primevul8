DEFUN (neighbor_disable_connected_check,
       neighbor_disable_connected_check_cmd,
       NEIGHBOR_CMD2 "disable-connected-check",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "one-hop away EBGP peer using loopback address\n")
{
  return peer_flag_set_vty (vty, argv[0], PEER_FLAG_DISABLE_CONNECTED_CHECK);
}