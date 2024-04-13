DEFUN (clear_ip_bgp_all_in_prefix_filter,
       clear_ip_bgp_all_in_prefix_filter_cmd,
       "clear ip bgp * in prefix-filter",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all peers\n"
       "Soft reconfig inbound update\n"
       "Push out prefix-list ORF and do inbound soft reconfig\n")
{
  if (argc== 1)
    return bgp_clear_vty (vty, argv[0], AFI_IP, SAFI_UNICAST, clear_all,
                          BGP_CLEAR_SOFT_IN_ORF_PREFIX, NULL);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_all,
			BGP_CLEAR_SOFT_IN_ORF_PREFIX, NULL);
}