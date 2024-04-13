DEFUN (no_bgp_bestpath_aspath_ignore,
       no_bgp_bestpath_aspath_ignore_cmd,
       "no bgp bestpath as-path ignore",
       NO_STR
       "BGP specific commands\n"
       "Change the default bestpath selection\n"
       "AS-path attribute\n"
       "Ignore as-path length in selecting a route\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_ASPATH_IGNORE);
  return CMD_SUCCESS;
}