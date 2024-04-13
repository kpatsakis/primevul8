DEFUN (bgp_redistribute_ipv4,
       bgp_redistribute_ipv4_cmd,
       "redistribute (connected|kernel|ospf|rip|static)",
       "Redistribute information from another routing protocol\n"
       "Connected\n"
       "Kernel routes\n"
       "Open Shurtest Path First (OSPF)\n"
       "Routing Information Protocol (RIP)\n"
       "Static routes\n")
{
  int type;

  type = bgp_str2route_type (AFI_IP, argv[0]);
  if (! type)
    {
      vty_out (vty, "%% Invalid route type%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  return bgp_redistribute_set (vty->index, AFI_IP, type);
}