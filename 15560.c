DEFUN (clear_ip_bgp_as_ipv4_soft,
       clear_ip_bgp_as_ipv4_soft_cmd,
       "clear ip bgp <1-65535> ipv4 (unicast|multicast) soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear peers with the AS number\n"
       "Address family\n"
       "Address Family Modifier\n"
       "Address Family Modifier\n"
       "Soft reconfig\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MULTICAST, clear_as,
			  BGP_CLEAR_SOFT_BOTH, argv[0]);

  return bgp_clear_vty (vty, NULL,AFI_IP, SAFI_UNICAST, clear_as,
			BGP_CLEAR_SOFT_BOTH, argv[0]);
}