DEFUN (clear_bgp_peer_soft,
       clear_bgp_peer_soft_cmd,
       "clear bgp (A.B.C.D|X:X::X:X) soft",
       CLEAR_STR
       BGP_STR
       "BGP neighbor address to clear\n"
       "BGP IPv6 neighbor to clear\n"
       "Soft reconfig\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_peer,
			BGP_CLEAR_SOFT_BOTH, argv[0]);
}