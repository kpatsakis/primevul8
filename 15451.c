DEFUN (no_bgp_deterministic_med,
       no_bgp_deterministic_med_cmd,
       "no bgp deterministic-med",
       NO_STR
       "BGP specific commands\n"
       "Pick the best-MED path among paths advertised from the neighboring AS\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_DETERMINISTIC_MED);
  return CMD_SUCCESS;
}