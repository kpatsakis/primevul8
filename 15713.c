DEFUN (bgp_confederation_identifier,
       bgp_confederation_identifier_cmd,
       "bgp confederation identifier <1-65535>",
       "BGP specific commands\n"
       "AS confederation parameters\n"
       "AS number\n"
       "Set routing domain confederation AS\n")
{
  struct bgp *bgp;
  as_t as;

  bgp = vty->index;

  VTY_GET_INTEGER ("AS", as, argv[0]);

  bgp_confederation_id_set (bgp, as);

  return CMD_SUCCESS;
}