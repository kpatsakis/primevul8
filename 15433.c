DEFUN (no_bgp_cluster_id,
       no_bgp_cluster_id_cmd,
       "no bgp cluster-id",
       NO_STR
       BGP_STR
       "Configure Route-Reflector Cluster-id\n")
{
  int ret;
  struct bgp *bgp;
  struct in_addr cluster;

  bgp = vty->index;

  if (argc == 1)
    {
      ret = inet_aton (argv[0], &cluster);
      if (! ret)
	{
	  vty_out (vty, "%% Malformed bgp cluster identifier%s", VTY_NEWLINE);
	  return CMD_WARNING;
	}
    }

  bgp_cluster_id_unset (bgp);

  return CMD_SUCCESS;
}