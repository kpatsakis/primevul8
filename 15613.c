DEFUN (no_neighbor_disable_connected_check,
       no_neighbor_disable_connected_check_cmd,
       NO_NEIGHBOR_CMD2 "disable-connected-check",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "one-hop away EBGP peer using loopback address\n")
{
  return peer_flag_unset_vty (vty, argv[0], PEER_FLAG_DISABLE_CONNECTED_CHECK);
}