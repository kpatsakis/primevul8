DEFUN (show_ip_bgp_paths, 
       show_ip_bgp_paths_cmd,
       "show ip bgp paths",
       SHOW_STR
       IP_STR
       BGP_STR
       "Path information\n")
{
  vty_out (vty, "Address Refcnt Path%s", VTY_NEWLINE);
  aspath_print_all_vty (vty);
  return CMD_SUCCESS;
}