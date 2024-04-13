DEFUN (bgp_bestpath_med,
       bgp_bestpath_med_cmd,
       "bgp bestpath med (confed|missing-as-worst)",
       "BGP specific commands\n"
       "Change the default bestpath selection\n"
       "MED attribute\n"
       "Compare MED among confederation paths\n"
       "Treat missing MED as the least preferred one\n")
{
  struct bgp *bgp;
  
  bgp = vty->index;

  if (strncmp (argv[0], "confed", 1) == 0)
    bgp_flag_set (bgp, BGP_FLAG_MED_CONFED);
  else
    bgp_flag_set (bgp, BGP_FLAG_MED_MISSING_AS_WORST);

  return CMD_SUCCESS;
}