DEFUN (show_ip_bgp_instance_ipv4_summary,
       show_ip_bgp_instance_ipv4_summary_cmd,
       "show ip bgp view WORD ipv4 (unicast|multicast) summary",
       SHOW_STR
       IP_STR
       BGP_STR
       "BGP view\n"
       "View name\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Summary of BGP neighbor status\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_show_summary_vty (vty, argv[0], AFI_IP, SAFI_MULTICAST);
  else
    return bgp_show_summary_vty (vty, argv[0], AFI_IP, SAFI_UNICAST);
}