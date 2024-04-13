DEFUN (show_ip_bgp_rsclient_summary,
       show_ip_bgp_rsclient_summary_cmd,
       "show ip bgp rsclient summary",
       SHOW_STR
       IP_STR
       BGP_STR
       "Information about Route Server Clients\n"
       "Summary of all Route Server Clients\n")
{
  return bgp_show_rsclient_summary_vty (vty, NULL, AFI_IP, SAFI_UNICAST);
}