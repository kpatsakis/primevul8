DEFUN (no_bgp_bestpath_med2,
       no_bgp_bestpath_med2_cmd,
       "no bgp bestpath med confed missing-as-worst",
       NO_STR
       "BGP specific commands\n"
       "Change the default bestpath selection\n"
       "MED attribute\n"
       "Compare MED among confederation paths\n"
       "Treat missing MED as the least preferred one\n")
{
  struct bgp *bgp;
  
  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_MED_CONFED);
  bgp_flag_unset (bgp, BGP_FLAG_MED_MISSING_AS_WORST);
  return CMD_SUCCESS;
}