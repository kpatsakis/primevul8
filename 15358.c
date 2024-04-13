DEFUN (clear_ip_bgp_external_soft,
       clear_ip_bgp_external_soft_cmd,
       "clear ip bgp external soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all external peers\n"
       "Soft reconfig\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_UNICAST, clear_external,
			BGP_CLEAR_SOFT_BOTH, NULL);
}