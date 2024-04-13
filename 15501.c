DEFUN (clear_bgp_all_rsclient,
       clear_bgp_all_rsclient_cmd,
       "clear bgp * rsclient",
       CLEAR_STR
       BGP_STR
       "Clear all peers\n"
       "Soft reconfig for rsclient RIB\n")
{
  if (argc == 1)
    return bgp_clear_vty (vty, argv[0], AFI_IP6, SAFI_UNICAST, clear_all,
                          BGP_CLEAR_SOFT_RSCLIENT, NULL);

  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_all,
                        BGP_CLEAR_SOFT_RSCLIENT, NULL);
}