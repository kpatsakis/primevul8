DEFUN (bgp_deterministic_med,
       bgp_deterministic_med_cmd,
       "bgp deterministic-med",
       "BGP specific commands\n"
       "Pick the best-MED path among paths advertised from the neighboring AS\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_set (bgp, BGP_FLAG_DETERMINISTIC_MED);
  return CMD_SUCCESS;
}