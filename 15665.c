DEFUN (neighbor_strict_capability,
       neighbor_strict_capability_cmd,
       NEIGHBOR_CMD "strict-capability-match",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "Strict capability negotiation match\n")
{
  return peer_flag_set_vty (vty, argv[0], PEER_FLAG_STRICT_CAP_MATCH);
}