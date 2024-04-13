DEFUN (clear_ip_bgp_instance_all_ipv4_in_prefix_filter,
       clear_ip_bgp_instance_all_ipv4_in_prefix_filter_cmd,
       "clear ip bgp view WORD * ipv4 (unicast|multicast) in prefix-filter",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all peers\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Soft reconfig inbound update\n"
       "Push out prefix-list ORF and do inbound soft reconfig\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_clear_vty (vty, argv[0], AFI_IP, SAFI_MULTICAST, clear_all,
                          BGP_CLEAR_SOFT_IN_ORF_PREFIX, NULL);

  return bgp_clear_vty (vty, argv[0], AFI_IP, SAFI_UNICAST, clear_all,
                        BGP_CLEAR_SOFT_IN_ORF_PREFIX, NULL);
}