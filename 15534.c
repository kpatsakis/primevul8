DEFUN (clear_ip_bgp_as_in_prefix_filter,
       clear_ip_bgp_as_in_prefix_filter_cmd,
       "clear ip bgp <1-65535> in prefix-filter",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear peers with the AS number\n"
       "Soft reconfig inbound update\n"
       "Push out prefix-list ORF and do inbound soft reconfig\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_as,
			BGP_CLEAR_SOFT_IN_ORF_PREFIX, argv[0]);
}