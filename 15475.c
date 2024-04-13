DEFUN (no_bgp_router_id,
       no_bgp_router_id_cmd,
       "no bgp router-id",
       NO_STR
       BGP_STR
       "Override configured router identifier\n")
{
  int ret;
  struct in_addr id;
  struct bgp *bgp;

  bgp = vty->index;

  if (argc == 1)
    {
      ret = inet_aton (argv[0], &id);
      if (! ret)
	{
	  vty_out (vty, "%% Malformed BGP router identifier%s", VTY_NEWLINE);
	  return CMD_WARNING;
	}

      if (! IPV4_ADDR_SAME (&bgp->router_id_static, &id))
	{
	  vty_out (vty, "%% BGP router-id doesn't match%s", VTY_NEWLINE);
	  return CMD_WARNING;
	}
    }

  bgp->router_id_static.s_addr = 0;
  bgp_router_id_set (bgp, &router_id_zebra);

  return CMD_SUCCESS;
}