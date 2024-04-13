DEFUN (show_ip_bgp_ipv4_rsclient_summary,
      show_ip_bgp_ipv4_rsclient_summary_cmd,
      "show ip bgp ipv4 (unicast|multicast) rsclient summary",
       SHOW_STR
       IP_STR
       BGP_STR
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Information about Route Server Clients\n"
       "Summary of all Route Server Clients\n")
{
  if (strncmp (argv[0], "m", 1) == 0)
    return bgp_show_rsclient_summary_vty (vty, NULL, AFI_IP, SAFI_MULTICAST);

  return bgp_show_rsclient_summary_vty (vty, NULL, AFI_IP, SAFI_UNICAST);
}