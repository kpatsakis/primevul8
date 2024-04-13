DEFUN (clear_bgp_peer_soft_in,
       clear_bgp_peer_soft_in_cmd,
       "clear bgp (A.B.C.D|X:X::X:X) soft in",
       CLEAR_STR
       BGP_STR
       "BGP neighbor address to clear\n"
       "BGP IPv6 neighbor to clear\n"
       "Soft reconfig\n"
       "Soft reconfig inbound update\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_peer,
			BGP_CLEAR_SOFT_IN, argv[0]);
}