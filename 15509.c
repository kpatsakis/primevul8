DEFUN (bgp_default_local_preference,
       bgp_default_local_preference_cmd,
       "bgp default local-preference <0-4294967295>",
       "BGP specific commands\n"
       "Configure BGP defaults\n"
       "local preference (higher=more preferred)\n"
       "Configure default local preference value\n")
{
  struct bgp *bgp;
  u_int32_t local_pref;

  bgp = vty->index;

  VTY_GET_INTEGER ("local preference", local_pref, argv[0]);

  bgp_default_local_preference_set (bgp, local_pref);

  return CMD_SUCCESS;
}