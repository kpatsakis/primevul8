DEFUN (show_ipv6_bgp_summary, 
       show_ipv6_bgp_summary_cmd,
       "show ipv6 bgp summary",
       SHOW_STR
       IPV6_STR
       BGP_STR
       "Summary of BGP neighbor status\n")
{
  return bgp_show_summary_vty (vty, NULL, AFI_IP6, SAFI_UNICAST);
}