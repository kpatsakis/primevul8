DEFUN (bgp_timers,
       bgp_timers_cmd,
       "timers bgp <0-65535> <0-65535>",
       "Adjust routing timers\n"
       "BGP timers\n"
       "Keepalive interval\n"
       "Holdtime\n")
{
  struct bgp *bgp;
  unsigned long keepalive = 0;
  unsigned long holdtime = 0;

  bgp = vty->index;

  VTY_GET_INTEGER ("keepalive", keepalive, argv[0]);
  VTY_GET_INTEGER ("holdtime", holdtime, argv[1]);

  /* Holdtime value check. */
  if (holdtime < 3 && holdtime != 0)
    {
      vty_out (vty, "%% hold time value must be either 0 or greater than 3%s",
	       VTY_NEWLINE);
      return CMD_WARNING;
    }

  bgp_timers_set (bgp, keepalive, holdtime);

  return CMD_SUCCESS;
}