DEFUN (no_bgp_log_neighbor_changes,
       no_bgp_log_neighbor_changes_cmd,
       "no bgp log-neighbor-changes",
       NO_STR
       "BGP specific commands\n"
       "Log neighbor up/down and reset reason\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_LOG_NEIGHBOR_CHANGES);
  return CMD_SUCCESS;
}