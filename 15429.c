DEFUN (clear_bgp_all_in_prefix_filter,
       clear_bgp_all_in_prefix_filter_cmd,
       "clear bgp * in prefix-filter",
       CLEAR_STR
       BGP_STR
       "Clear all peers\n"
       "Soft reconfig inbound update\n"
       "Push out prefix-list ORF and do inbound soft reconfig\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_all,
			BGP_CLEAR_SOFT_IN_ORF_PREFIX, NULL);
}