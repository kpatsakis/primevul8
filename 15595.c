DEFUN (bgp_redistribute_ipv6,
       bgp_redistribute_ipv6_cmd,
       "redistribute (connected|kernel|ospf6|ripng|static)",
       "Redistribute information from another routing protocol\n"
       "Connected\n"
       "Kernel routes\n"
       "Open Shurtest Path First (OSPFv3)\n"
       "Routing Information Protocol (RIPng)\n"
       "Static routes\n")
{
  int type;

  type = bgp_str2route_type (AFI_IP6, argv[0]);
  if (! type)
    {
      vty_out (vty, "%% Invalid route type%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  return bgp_redistribute_set (vty->index, AFI_IP6, type);
}