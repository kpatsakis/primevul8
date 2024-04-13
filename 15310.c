DEFUN (clear_ip_bgp_peer_group_soft_in,
       clear_ip_bgp_peer_group_soft_in_cmd,
       "clear ip bgp peer-group WORD soft in",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all members of peer-group\n"
       "BGP peer-group name\n"
       "Soft reconfig\n"
       "Soft reconfig inbound update\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_group,
			BGP_CLEAR_SOFT_IN, argv[0]);
}