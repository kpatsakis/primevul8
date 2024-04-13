DEFUN (clear_ip_bgp_external_ipv4_soft,
       clear_ip_bgp_external_ipv4_soft_cmd,
       "clear ip bgp external ipv4 (unicast|multicast) soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all external peers\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Soft reconfig\n")
{
  if (strncmp (argv[0], "m", 1) == 0)
    return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MULTICAST, clear_external,
			  BGP_CLEAR_SOFT_BOTH, NULL);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_external,
			BGP_CLEAR_SOFT_BOTH, NULL);
}