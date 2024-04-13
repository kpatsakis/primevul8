DEFUN (no_bgp_timers,
       no_bgp_timers_cmd,
       "no timers bgp",
       NO_STR
       "Adjust routing timers\n"
       "BGP timers\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_timers_unset (bgp);

  return CMD_SUCCESS;
}