DEFUN (no_neighbor_passive,
       no_neighbor_passive_cmd,
       NO_NEIGHBOR_CMD2 "passive",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Don't send open messages to this neighbor\n")
{
  return peer_flag_unset_vty (vty, argv[0], PEER_FLAG_PASSIVE);
}