DEFUN (clear_ip_bgp_peer_group_ipv4_soft,
       clear_ip_bgp_peer_group_ipv4_soft_cmd,
       "clear ip bgp peer-group WORD ipv4 (unicast|multicast) soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all members of peer-group\n"
       "BGP peer-group name\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Soft reconfig\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MULTICAST, clear_group,
			  BGP_CLEAR_SOFT_BOTH, argv[0]);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_group,
			BGP_CLEAR_SOFT_BOTH, argv[0]);
}