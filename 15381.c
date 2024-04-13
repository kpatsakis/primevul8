DEFUN (clear_ip_bgp_as_soft_in,
       clear_ip_bgp_as_soft_in_cmd,
       "clear ip bgp <1-65535> soft in",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear peers with the AS number\n"
       "Soft reconfig\n"
       "Soft reconfig inbound update\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_as,
			BGP_CLEAR_SOFT_IN, argv[0]);
}