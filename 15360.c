DEFUN (bgp_graceful_restart_stalepath_time,
       bgp_graceful_restart_stalepath_time_cmd,
       "bgp graceful-restart stalepath-time <1-3600>",
       "BGP specific commands\n"
       "Graceful restart capability parameters\n"
       "Set the max time to hold onto restarting peer's stale paths\n"
       "Delay value (seconds)\n")
{
  struct bgp *bgp;
  u_int32_t stalepath;

  bgp = vty->index;
  if (! bgp)
    return CMD_WARNING;

  VTY_GET_INTEGER_RANGE ("stalepath-time", stalepath, argv[0], 1, 3600);
  bgp->stalepath_time = stalepath;
  return CMD_SUCCESS;
}