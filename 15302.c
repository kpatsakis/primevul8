DEFUN (show_ip_bgp_instance_ipv4_rsclient_summary,
      show_ip_bgp_instance_ipv4_rsclient_summary_cmd,
      "show ip bgp view WORD ipv4 (unicast|multicast) rsclient summary",
       SHOW_STR
       IP_STR
       BGP_STR
       "BGP view\n"
       "View name\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Information about Route Server Clients\n"
       "Summary of all Route Server Clients\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_show_rsclient_summary_vty (vty, argv[0], AFI_IP, SAFI_MULTICAST);

  return bgp_show_rsclient_summary_vty (vty, argv[0], AFI_IP, SAFI_UNICAST);
}