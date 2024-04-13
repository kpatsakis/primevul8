DEFUN (neighbor_passive,
       neighbor_passive_cmd,
       NEIGHBOR_CMD2 "passive",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Don't send open messages to this neighbor\n")
{
  return peer_flag_set_vty (vty, argv[0], PEER_FLAG_PASSIVE);
}