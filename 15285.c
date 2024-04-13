DEFUN (clear_ip_bgp_peer_group,
       clear_ip_bgp_peer_group_cmd, 
       "clear ip bgp peer-group WORD",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all members of peer-group\n"
       "BGP peer-group name\n")
{
  return bgp_clear_vty (vty, NULL, 0, 0, clear_group, BGP_CLEAR_SOFT_NONE, argv[0]);
}