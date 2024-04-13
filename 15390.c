DEFUN (clear_bgp_peer_in_prefix_filter,
       clear_bgp_peer_in_prefix_filter_cmd,
       "clear bgp (A.B.C.D|X:X::X:X) in prefix-filter",
       CLEAR_STR
       BGP_STR
       "BGP neighbor address to clear\n"
       "BGP IPv6 neighbor to clear\n"
       "Soft reconfig inbound update\n"
       "Push out the existing ORF prefix-list\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_peer,
			BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);
}