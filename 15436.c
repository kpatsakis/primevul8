DEFUN (clear_ip_bgp_as_ipv4_soft_in,
       clear_ip_bgp_as_ipv4_soft_in_cmd,
       "clear ip bgp <1-65535> ipv4 (unicast|multicast) soft in",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear peers with the AS number\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Soft reconfig\n"
       "Soft reconfig inbound update\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MULTICAST, clear_as,
			  BGP_CLEAR_SOFT_IN, argv[0]);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_as,
			BGP_CLEAR_SOFT_IN, argv[0]);
}