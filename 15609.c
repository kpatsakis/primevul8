DEFUN (clear_ip_bgp_peer_ipv4_soft_out,
       clear_ip_bgp_peer_ipv4_soft_out_cmd,
       "clear ip bgp A.B.C.D ipv4 (unicast|multicast) soft out",
       CLEAR_STR
       IP_STR
       BGP_STR
       "BGP neighbor address to clear\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Soft reconfig\n"
       "Soft reconfig outbound update\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MULTICAST, clear_peer,
			  BGP_CLEAR_SOFT_OUT, argv[0]);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_peer,
			BGP_CLEAR_SOFT_OUT, argv[0]);
}