DEFUN (no_bgp_confederation_identifier,
       no_bgp_confederation_identifier_cmd,
       "no bgp confederation identifier",
       NO_STR
       "BGP specific commands\n"
       "AS confederation parameters\n"
       "AS number\n")
{
  struct bgp *bgp;
  as_t as;

  bgp = vty->index;

  if (argc == 1)
    VTY_GET_INTEGER ("AS", as, argv[0]);

  bgp_confederation_id_unset (bgp);

  return CMD_SUCCESS;
}