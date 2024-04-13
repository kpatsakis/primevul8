DEFUN (clear_ip_bgp_all_soft,
       clear_ip_bgp_all_soft_cmd,
       "clear ip bgp * soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all peers\n"
       "Soft reconfig\n")
{
  if (argc == 1)
    return bgp_clear_vty (vty, argv[0], AFI_IP, SAFI_UNICAST, clear_all,
                        BGP_CLEAR_SOFT_BOTH, NULL);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_all,
			BGP_CLEAR_SOFT_BOTH, NULL);
}