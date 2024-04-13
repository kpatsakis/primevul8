DEFUN (no_neighbor_override_capability,
       no_neighbor_override_capability_cmd,
       NO_NEIGHBOR_CMD2 "override-capability",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Override capability negotiation result\n")
{
  return peer_flag_unset_vty (vty, argv[0], PEER_FLAG_OVERRIDE_CAPABILITY);
}