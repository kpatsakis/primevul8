DEFUN (bgp_confederation_peers,
       bgp_confederation_peers_cmd,
       "bgp confederation peers .<1-65535>",
       "BGP specific commands\n"
       "AS confederation parameters\n"
       "Peer ASs in BGP confederation\n"
       AS_STR)
{
  struct bgp *bgp;
  as_t as;
  int i;

  bgp = vty->index;

  for (i = 0; i < argc; i++)
    {
      VTY_GET_INTEGER_RANGE ("AS", as, argv[i], 1, 65535);

      if (bgp->as == as)
	{
	  vty_out (vty, "%% Local member-AS not allowed in confed peer list%s",
		   VTY_NEWLINE);
	  continue;
	}

      bgp_confederation_peers_add (bgp, as);
    }
  return CMD_SUCCESS;
}