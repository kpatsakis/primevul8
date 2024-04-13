DEFUN (clear_ip_bgp_peer_group_ipv4_in_prefix_filter,
       clear_ip_bgp_peer_group_ipv4_in_prefix_filter_cmd,
       "clear ip bgp peer-group WORD ipv4 (unicast|multicast) in prefix-filter",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all members of peer-group\n"
       "BGP peer-group name\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Soft reconfig inbound update\n"
       "Push out prefix-list ORF and do inbound soft reconfig\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MULTICAST, clear_group,
			  BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_group,
			BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);
}