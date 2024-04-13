DEFUN (neighbor_timers_connect,
       neighbor_timers_connect_cmd,
       NEIGHBOR_CMD "timers connect <0-65535>",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "BGP per neighbor timers\n"
       "BGP connect timer\n"
       "Connect timer\n")
{
  return peer_timers_connect_set_vty (vty, argv[0], argv[1]);
}