DEFUN (router_bgp, 
       router_bgp_cmd, 
       "router bgp <1-65535>",
       ROUTER_STR
       BGP_STR
       AS_STR)
{
  int ret;
  as_t as;
  struct bgp *bgp;
  const char *name = NULL;

  VTY_GET_INTEGER_RANGE ("AS", as, argv[0], 1, 65535);

  if (argc == 2)
    name = argv[1];

  ret = bgp_get (&bgp, &as, name);
  switch (ret)
    {
    case BGP_ERR_MULTIPLE_INSTANCE_NOT_SET:
      vty_out (vty, "Please specify 'bgp multiple-instance' first%s", 
	       VTY_NEWLINE);
      return CMD_WARNING;
    case BGP_ERR_AS_MISMATCH:
      vty_out (vty, "BGP is already running; AS is %d%s", as, VTY_NEWLINE);
      return CMD_WARNING;
    case BGP_ERR_INSTANCE_MISMATCH:
      vty_out (vty, "BGP view name and AS number mismatch%s", VTY_NEWLINE);
      vty_out (vty, "BGP instance is already running; AS is %d%s",
	       as, VTY_NEWLINE);
      return CMD_WARNING;
    }

  vty->node = BGP_NODE;
  vty->index = bgp;

  return CMD_SUCCESS;
}