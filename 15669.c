DEFUN (no_bgp_enforce_first_as,
       no_bgp_enforce_first_as_cmd,
       "no bgp enforce-first-as",
       NO_STR
       BGP_STR
       "Enforce the first AS for EBGP routes\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_unset (bgp, BGP_FLAG_ENFORCE_FIRST_AS);
  return CMD_SUCCESS;
}