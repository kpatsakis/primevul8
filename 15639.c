DEFUN (clear_ip_bgp_peer_soft,
       clear_ip_bgp_peer_soft_cmd,
       "clear ip bgp A.B.C.D soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "BGP neighbor address to clear\n"
       "Soft reconfig\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_peer,
			BGP_CLEAR_SOFT_BOTH, argv[0]);
}