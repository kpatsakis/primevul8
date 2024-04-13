DEFUN (neighbor_timers,
       neighbor_timers_cmd,
       NEIGHBOR_CMD2 "timers <0-65535> <0-65535>",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "BGP per neighbor timers\n"
       "Keepalive interval\n"
       "Holdtime\n")
{
  return peer_timers_set_vty (vty, argv[0], argv[1], argv[2]);
}