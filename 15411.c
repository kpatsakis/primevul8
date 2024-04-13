DEFUN (show_bgp_summary, 
       show_bgp_summary_cmd,
       "show bgp summary",
       SHOW_STR
       BGP_STR
       "Summary of BGP neighbor status\n")
{
  return bgp_show_summary_vty (vty, NULL, AFI_IP6, SAFI_UNICAST);
}