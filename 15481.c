DEFUN (bgp_always_compare_med,
       bgp_always_compare_med_cmd,
       "bgp always-compare-med",
       "BGP specific commands\n"
       "Allow comparing MED from different neighbors\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_set (bgp, BGP_FLAG_ALWAYS_COMPARE_MED);
  return CMD_SUCCESS;
}