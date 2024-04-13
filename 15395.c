DEFUN (bgp_redistribute_ipv4_rmap_metric,
       bgp_redistribute_ipv4_rmap_metric_cmd,
       "redistribute (connected|kernel|ospf|rip|static) route-map WORD metric <0-4294967295>",
       "Redistribute information from another routing protocol\n"
       "Connected\n"
       "Kernel routes\n"
       "Open Shurtest Path First (OSPF)\n"
       "Routing Information Protocol (RIP)\n"
       "Static routes\n"
       "Route map reference\n"
       "Pointer to route-map entries\n"
       "Metric for redistributed routes\n"
       "Default metric\n")
{
  int type;
  u_int32_t metric;

  type = bgp_str2route_type (AFI_IP, argv[0]);
  if (! type)
    {
      vty_out (vty, "%% Invalid route type%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  VTY_GET_INTEGER ("metric", metric, argv[2]);

  bgp_redistribute_rmap_set (vty->index, AFI_IP, type, argv[1]);
  bgp_redistribute_metric_set (vty->index, AFI_IP, type, metric);
  return bgp_redistribute_set (vty->index, AFI_IP, type);
}