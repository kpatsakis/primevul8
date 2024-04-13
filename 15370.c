DEFUN (no_bgp_graceful_restart,
       no_bgp_graceful_restart_cmd,
       "no bgp graceful-restart",
       NO_STR
       "BGP specific commands\n"
       "Graceful restart capability parameters\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_GRACEFUL_RESTART);
  return CMD_SUCCESS;
}