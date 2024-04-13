DEFUN (no_bgp_default_local_preference,
       no_bgp_default_local_preference_cmd,
       "no bgp default local-preference",
       NO_STR
       "BGP specific commands\n"
       "Configure BGP defaults\n"
       "local preference (higher=more preferred)\n")
{
  struct bgp *bgp;

  bgp = vty->index;
  bgp_default_local_preference_unset (bgp);
  return CMD_SUCCESS;
}