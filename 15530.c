DEFUN (clear_ip_bgp_all_ipv4_soft,
       clear_ip_bgp_all_ipv4_soft_cmd,
       "clear ip bgp * ipv4 (unicast|multicast) soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all peers\n"
       "Address family\n"
       "Address Family Modifier\n"
       "Address Family Modifier\n"
       "Soft reconfig\n")
{
  if (strncmp (argv[0], "m", 1) == 0)
    return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MULTICAST, clear_all,
			  BGP_CLEAR_SOFT_BOTH, NULL);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_all,
			BGP_CLEAR_SOFT_BOTH, NULL);
}