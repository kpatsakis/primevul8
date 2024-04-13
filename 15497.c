DEFUN (no_bgp_graceful_restart_stalepath_time,
       no_bgp_graceful_restart_stalepath_time_cmd,
       "no bgp graceful-restart stalepath-time",
       NO_STR
       "BGP specific commands\n"
       "Graceful restart capability parameters\n"
       "Set the max time to hold onto restarting peer's stale paths\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  if (! bgp)
    return CMD_WARNING;

  bgp->stalepath_time = BGP_DEFAULT_STALEPATH_TIME;
  return CMD_SUCCESS;
}