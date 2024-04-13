DEFUN (bgp_router_id,
       bgp_router_id_cmd,
       "bgp router-id A.B.C.D",
       BGP_STR
       "Override configured router identifier\n"
       "Manually configured router identifier\n")
{
  int ret;
  struct in_addr id;
  struct bgp *bgp;

  bgp = vty->index;

  ret = inet_aton (argv[0], &id);
  if (! ret)
    {
      vty_out (vty, "%% Malformed bgp router identifier%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  bgp->router_id_static = id;
  bgp_router_id_set (bgp, &id);

  return CMD_SUCCESS;
}