DEFUN (bgp_log_neighbor_changes,
       bgp_log_neighbor_changes_cmd,
       "bgp log-neighbor-changes",
       "BGP specific commands\n"
       "Log neighbor up/down and reset reason\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_set (bgp, BGP_FLAG_LOG_NEIGHBOR_CHANGES);
  return CMD_SUCCESS;
}