DEFUN (clear_ip_bgp_all_soft_in,
       clear_ip_bgp_all_soft_in_cmd,
       "clear ip bgp * soft in",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all peers\n"
       "Soft reconfig\n"
       "Soft reconfig inbound update\n")
{
  if (argc == 1)
    return bgp_clear_vty (vty, argv[0], AFI_IP, SAFI_UNICAST, clear_all,
                          BGP_CLEAR_SOFT_IN, NULL);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_all,
			BGP_CLEAR_SOFT_IN, NULL);
}