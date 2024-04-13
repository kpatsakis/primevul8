DEFUN (show_ip_bgp_attr_info, 
       show_ip_bgp_attr_info_cmd,
       "show ip bgp attribute-info",
       SHOW_STR
       IP_STR
       BGP_STR
       "List all bgp attribute information\n")
{
  attr_show_all (vty);
  return CMD_SUCCESS;
}