DEFUN (clear_bgp_peer_rsclient,
       clear_bgp_peer_rsclient_cmd,
       "clear bgp (A.B.C.D|X:X::X:X) rsclient",
       CLEAR_STR
       BGP_STR
       "BGP neighbor IP address to clear\n"
       "BGP IPv6 neighbor to clear\n"
       "Soft reconfig for rsclient RIB\n")
{
  if (argc == 2)
    return bgp_clear_vty (vty, argv[0], AFI_IP6, SAFI_UNICAST, clear_peer,
                          BGP_CLEAR_SOFT_RSCLIENT, argv[1]);

  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_peer,
                        BGP_CLEAR_SOFT_RSCLIENT, argv[0]);
}