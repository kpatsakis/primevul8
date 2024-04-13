DEFUN (show_ip_bgp_ipv4_summary, 
       show_ip_bgp_ipv4_summary_cmd,
       "show ip bgp ipv4 (unicast|multicast) summary",
       SHOW_STR
       IP_STR
       BGP_STR
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Summary of BGP neighbor status\n")
{
  if (strncmp (argv[0], "m", 1) == 0)
    return bgp_show_summary_vty (vty, NULL, AFI_IP, SAFI_MULTICAST);

  return bgp_show_summary_vty (vty, NULL, AFI_IP, SAFI_UNICAST);
}