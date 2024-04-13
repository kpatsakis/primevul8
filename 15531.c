DEFUN (show_bgp_instance_summary,
       show_bgp_instance_summary_cmd,
       "show bgp view WORD summary",
       SHOW_STR
       BGP_STR
       "BGP view\n"
       "View name\n"
       "Summary of BGP neighbor status\n")
{
  return bgp_show_summary_vty (vty, argv[0], AFI_IP6, SAFI_UNICAST);
}