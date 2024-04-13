DEFUN (no_neighbor_capability_dynamic,
       no_neighbor_capability_dynamic_cmd,
       NO_NEIGHBOR_CMD2 "capability dynamic",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Advertise capability to the peer\n"
       "Advertise dynamic capability to this neighbor\n")
{
  return peer_flag_unset_vty (vty, argv[0], PEER_FLAG_DYNAMIC_CAPABILITY);
}