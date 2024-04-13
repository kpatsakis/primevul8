DEFUN (show_bgp_rsclient_summary,
       show_bgp_rsclient_summary_cmd,
       "show bgp rsclient summary",
       SHOW_STR
       BGP_STR
       "Information about Route Server Clients\n"
       "Summary of all Route Server Clients\n")
{
  return bgp_show_rsclient_summary_vty (vty, NULL, AFI_IP6, SAFI_UNICAST);
}