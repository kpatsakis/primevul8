DEFUN (neighbor_override_capability,
       neighbor_override_capability_cmd,
       NEIGHBOR_CMD2 "override-capability",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Override capability negotiation result\n")
{
  return peer_flag_set_vty (vty, argv[0], PEER_FLAG_OVERRIDE_CAPABILITY);
}