DEFUN (show_ipv6_mbgp_summary, 
       show_ipv6_mbgp_summary_cmd,
       "show ipv6 mbgp summary",
       SHOW_STR
       IPV6_STR
       MBGP_STR
       "Summary of BGP neighbor status\n")
{
  return bgp_show_summary_vty (vty, NULL, AFI_IP6, SAFI_MULTICAST);
}