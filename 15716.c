DEFUN (no_neighbor_timers_connect,
       no_neighbor_timers_connect_cmd,
       NO_NEIGHBOR_CMD "timers connect",
       NO_STR
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "BGP per neighbor timers\n"
       "BGP connect timer\n")
{
  return peer_timers_connect_unset_vty (vty, argv[0]);
}