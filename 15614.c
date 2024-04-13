DEFUN (no_bgp_redistribute_ipv6_metric,
       no_bgp_redistribute_ipv6_metric_cmd,
       "no redistribute (connected|kernel|ospf6|ripng|static) metric <0-4294967295>",
       NO_STR
       "Redistribute information from another routing protocol\n"
       "Connected\n"
       "Kernel routes\n"
       "Open Shurtest Path First (OSPFv3)\n"
       "Routing Information Protocol (RIPng)\n"
       "Static routes\n"
       "Metric for redistributed routes\n"
       "Default metric\n")
{
  int type;

  type = bgp_str2route_type (AFI_IP6, argv[0]);
  if (! type)
    {
      vty_out (vty, "%% Invalid route type%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  bgp_redistribute_metric_unset (vty->index, AFI_IP6, type);
  return CMD_SUCCESS;
}