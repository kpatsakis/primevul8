DEFUN (bgp_enforce_first_as,
       bgp_enforce_first_as_cmd,
       "bgp enforce-first-as",
       BGP_STR
       "Enforce the first AS for EBGP routes\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_flag_set (bgp, BGP_FLAG_ENFORCE_FIRST_AS);
  return CMD_SUCCESS;
}