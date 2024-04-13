DEFUN (neighbor_dont_capability_negotiate,
       neighbor_dont_capability_negotiate_cmd,
       NEIGHBOR_CMD2 "dont-capability-negotiate",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Do not perform capability negotiation\n")
{
  return peer_flag_set_vty (vty, argv[0], PEER_FLAG_DONT_CAPABILITY);
}