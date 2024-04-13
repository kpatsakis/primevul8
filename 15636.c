DEFUN (clear_bgp_all_soft,
       clear_bgp_all_soft_cmd,
       "clear bgp * soft",
       CLEAR_STR
       BGP_STR
       "Clear all peers\n"
       "Soft reconfig\n")
{
  if (argc == 1)
    return bgp_clear_vty (vty, argv[0], AFI_IP6, SAFI_UNICAST, clear_all,
                        BGP_CLEAR_SOFT_BOTH, argv[0]);
 
  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_all,
			BGP_CLEAR_SOFT_BOTH, argv[0]);
}