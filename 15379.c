DEFUN (show_ip_bgp_ipv4_paths, 
       show_ip_bgp_ipv4_paths_cmd,
       "show ip bgp ipv4 (unicast|multicast) paths",
       SHOW_STR
       IP_STR
       BGP_STR
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Path information\n")
{
  vty_out (vty, "Address Refcnt Path\r\n");
  aspath_print_all_vty (vty);

  return CMD_SUCCESS;
}