DEFUN (clear_ip_bgp_peer_in_prefix_filter,
       clear_ip_bgp_peer_in_prefix_filter_cmd,
       "clear ip bgp A.B.C.D in prefix-filter",
       CLEAR_STR
       IP_STR
       BGP_STR
       "BGP neighbor address to clear\n"
       "Soft reconfig inbound update\n"
       "Push out the existing ORF prefix-list\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_peer,
			BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);
}