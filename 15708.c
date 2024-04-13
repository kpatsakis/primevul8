DEFUN (show_ip_bgp_community_info, 
       show_ip_bgp_community_info_cmd,
       "show ip bgp community-info",
       SHOW_STR
       IP_STR
       BGP_STR
       "List all bgp community information\n")
{
  vty_out (vty, "Address Refcnt Community%s", VTY_NEWLINE);

  hash_iterate (community_hash (), 
		(void (*) (struct hash_backet *, void *))
		community_show_all_iterator,
		vty);

  return CMD_SUCCESS;
}