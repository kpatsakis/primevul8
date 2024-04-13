DEFUN (clear_ip_bgp_all_rsclient,
       clear_ip_bgp_all_rsclient_cmd,
       "clear ip bgp * rsclient",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all peers\n"
       "Soft reconfig for rsclient RIB\n")
{
  if (argc == 1)
    return bgp_clear_vty (vty, argv[0], AFI_IP, SAFI_UNICAST, clear_all,
                          BGP_CLEAR_SOFT_RSCLIENT, NULL);

  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_all,
                        BGP_CLEAR_SOFT_RSCLIENT, NULL);
}