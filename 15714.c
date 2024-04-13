DEFUN (clear_ip_bgp_external_soft_in,
       clear_ip_bgp_external_soft_in_cmd,
       "clear ip bgp external soft in",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all external peers\n"
       "Soft reconfig\n"
       "Soft reconfig inbound update\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_external,
			BGP_CLEAR_SOFT_IN, NULL);
}