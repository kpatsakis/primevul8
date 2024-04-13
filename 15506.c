DEFUN (clear_ip_bgp_instance_all_ipv4_soft_out,
       clear_ip_bgp_instance_all_ipv4_soft_out_cmd,
       "clear ip bgp view WORD * ipv4 (unicast|multicast) soft out",
       CLEAR_STR
       IP_STR
       BGP_STR
       "BGP view\n"
       "view name\n"
       "Clear all peers\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n"
       "Soft reconfig outbound update\n")
{
  if (strncmp (argv[1], "m", 1) == 0)
    return bgp_clear_vty (vty, argv[0], AFI_IP, SAFI_MULTICAST, clear_all,
                          BGP_CLEAR_SOFT_OUT, NULL);

  return bgp_clear_vty (vty, argv[0], AFI_IP, SAFI_UNICAST, clear_all,
                        BGP_CLEAR_SOFT_OUT, NULL);
}