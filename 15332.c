DEFUN (clear_ip_bgp_peer,
       clear_ip_bgp_peer_cmd, 
       "clear ip bgp (A.B.C.D|X:X::X:X)",
       CLEAR_STR
       IP_STR
       BGP_STR
       "BGP neighbor IP address to clear\n"
       "BGP IPv6 neighbor to clear\n")
{
  return bgp_clear_vty (vty, NULL, 0, 0, clear_peer, BGP_CLEAR_SOFT_NONE, argv[0]);
}