DEFUN (clear_ip_bgp_peer_group_in_prefix_filter,
       clear_ip_bgp_peer_group_in_prefix_filter_cmd,
       "clear ip bgp peer-group WORD in prefix-filter",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all members of peer-group\n"
       "BGP peer-group name\n"
       "Soft reconfig inbound update\n"
       "Push out prefix-list ORF and do inbound soft reconfig\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_group,
			BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);
}