DEFUN (bgp_redistribute_ipv4_metric_rmap,
       bgp_redistribute_ipv4_metric_rmap_cmd,
       "redistribute (connected|kernel|ospf|rip|static) metric <0-4294967295> route-map WORD",
       "Redistribute information from another routing protocol\n"
       "Connected\n"
       "Kernel routes\n"
       "Open Shurtest Path First (OSPF)\n"
       "Routing Information Protocol (RIP)\n"
       "Static routes\n"
       "Metric for redistributed routes\n"
       "Default metric\n"
       "Route map reference\n"
       "Pointer to route-map entries\n")
{
  int type;
  u_int32_t metric;

  type = bgp_str2route_type (AFI_IP, argv[0]);
  if (! type)
    {
      vty_out (vty, "%% Invalid route type%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  VTY_GET_INTEGER ("metric", metric, argv[1]);

  bgp_redistribute_metric_set (vty->index, AFI_IP, type, metric);
  bgp_redistribute_rmap_set (vty->index, AFI_IP, type, argv[2]);
  return bgp_redistribute_set (vty->index, AFI_IP, type);
}