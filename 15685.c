DEFUN (bgp_graceful_restart,
       bgp_graceful_restart_cmd,
       "bgp graceful-restart",
       "BGP specific commands\n"
       "Graceful restart capability parameters\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_set (bgp, BGP_FLAG_GRACEFUL_RESTART);
  return CMD_SUCCESS;
}