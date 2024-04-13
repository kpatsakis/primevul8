DEFUN (no_neighbor_timers,
       no_neighbor_timers_cmd,
       NO_NEIGHBOR_CMD2 "timers",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "BGP per neighbor timers\n")
{
  return peer_timers_unset_vty (vty, argv[0]);
}