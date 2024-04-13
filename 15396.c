DEFUN (no_bgp_redistribute_ipv4_rmap,
       no_bgp_redistribute_ipv4_rmap_cmd,
       "no redistribute (connected|kernel|ospf|rip|static) route-map WORD",
       NO_STR
       "Redistribute information from another routing protocol\n"
       "Connected\n"
       "Kernel routes\n"
       "Open Shurtest Path First (OSPF)\n"
       "Routing Information Protocol (RIP)\n"
       "Static routes\n"
       "Route map reference\n"
       "Pointer to route-map entries\n")
{
  int type;

  type = bgp_str2route_type (AFI_IP, argv[0]);
  if (! type)
    {
      vty_out (vty, "%% Invalid route type%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  bgp_redistribute_routemap_unset (vty->index, AFI_IP, type);
  return CMD_SUCCESS;
}