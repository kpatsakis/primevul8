DEFUN (neighbor_capability_dynamic,
       neighbor_capability_dynamic_cmd,
       NEIGHBOR_CMD2 "capability dynamic",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Advertise capability to the peer\n"
       "Advertise dynamic capability to this neighbor\n")
{
  return peer_flag_set_vty (vty, argv[0], PEER_FLAG_DYNAMIC_CAPABILITY);
}