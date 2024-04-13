DEFUN (clear_bgp_all_soft_out,
       clear_bgp_all_soft_out_cmd,
       "clear bgp * soft out",
       CLEAR_STR
       BGP_STR
       "Clear all peers\n"
       "Soft reconfig\n"
       "Soft reconfig outbound update\n")
{
  if (argc == 1)
    return bgp_clear_vty (vty, argv[0], AFI_IP6, SAFI_UNICAST, clear_all,
                          BGP_CLEAR_SOFT_OUT, NULL);

  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_all,
			BGP_CLEAR_SOFT_OUT, NULL);
}