DEFUN (no_bgp_confederation_peers,
       no_bgp_confederation_peers_cmd,
       "no bgp confederation peers .<1-65535>",
       NO_STR
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
      
      bgp_confederation_peers_remove (bgp, as);
    }
  return CMD_SUCCESS;
}