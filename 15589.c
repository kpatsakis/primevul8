DEFUN (no_bgp_always_compare_med,
       no_bgp_always_compare_med_cmd,
       "no bgp always-compare-med",
       NO_STR
       "BGP specific commands\n"
       "Allow comparing MED from different neighbors\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_ALWAYS_COMPARE_MED);
  return CMD_SUCCESS;
}