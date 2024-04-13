DEFUN (no_neighbor_strict_capability,
       no_neighbor_strict_capability_cmd,
       NO_NEIGHBOR_CMD "strict-capability-match",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "Strict capability negotiation match\n")
{
  return peer_flag_unset_vty (vty, argv[0], PEER_FLAG_STRICT_CAP_MATCH);
}