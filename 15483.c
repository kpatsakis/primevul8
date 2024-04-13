DEFUN (clear_ip_bgp_as_ipv4_in_prefix_filter,
       clear_ip_bgp_as_ipv4_in_prefix_filter_cmd,
       "clear ip bgp <1-65535> ipv4 (unicast|multicast) in prefix-filter",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear peers with the AS number\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Soft reconfig inbound update\n"
       "Push out prefix-list ORF and do inbound soft reconfig\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MULTICAST, clear_as,
			  BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_as,
			BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);
}