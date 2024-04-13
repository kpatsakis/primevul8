DEFUN (clear_ip_bgp_peer_ipv4_in_prefix_filter,
       clear_ip_bgp_peer_ipv4_in_prefix_filter_cmd,
       "clear ip bgp A.B.C.D ipv4 (unicast|multicast) in prefix-filter",
       CLEAR_STR
       IP_STR
       BGP_STR
       "BGP neighbor address to clear\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Soft reconfig inbound update\n"
       "Push out the existing ORF prefix-list\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MULTICAST, clear_peer,
			  BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_peer,
			BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);
}