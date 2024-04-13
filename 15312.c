DEFUN (no_router_bgp,
       no_router_bgp_cmd,
       "no router bgp <1-65535>",
       NO_STR
       ROUTER_STR
       BGP_STR
       AS_STR)
{
  as_t as;
  struct bgp *bgp;
  const char *name = NULL;

  VTY_GET_INTEGER_RANGE ("AS", as, argv[0], 1, 65535);

  if (argc == 2)
    name = argv[1];

  /* Lookup bgp structure. */
  bgp = bgp_lookup (as, name);
  if (! bgp)
    {
      vty_out (vty, "%% Can't find BGP instance%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  bgp_delete (bgp);

  return CMD_SUCCESS;
}