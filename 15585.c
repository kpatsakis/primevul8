DEFUN (bgp_cluster_id,
       bgp_cluster_id_cmd,
       "bgp cluster-id A.B.C.D",
       BGP_STR
       "Configure Route-Reflector Cluster-id\n"
       "Route-Reflector Cluster-id in IP address format\n")
{
  int ret;
  struct bgp *bgp;
  struct in_addr cluster;

  bgp = vty->index;

  ret = inet_aton (argv[0], &cluster);
  if (! ret)
    {
      vty_out (vty, "%% Malformed bgp cluster identifier%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  bgp_cluster_id_set (bgp, &cluster);

  return CMD_SUCCESS;
}