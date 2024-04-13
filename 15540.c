DEFUN (show_bgp_instance_rsclient_summary,
       show_bgp_instance_rsclient_summary_cmd,
       "show bgp view WORD rsclient summary",
       SHOW_STR
       BGP_STR
       "BGP view\n"
       "View name\n"
       "Information about Route Server Clients\n"
       "Summary of all Route Server Clients\n")
{
  return bgp_show_rsclient_summary_vty (vty, argv[0], AFI_IP6, SAFI_UNICAST);
}