DEFUN (clear_ip_bgp_peer_group_soft,
       clear_ip_bgp_peer_group_soft_cmd,
       "clear ip bgp peer-group WORD soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all members of peer-group\n"
       "BGP peer-group name\n"
       "Soft reconfig\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_group,
			BGP_CLEAR_SOFT_BOTH, argv[0]);
}