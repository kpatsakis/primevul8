DEFUN (clear_bgp_external_soft_out,
       clear_bgp_external_soft_out_cmd,
       "clear bgp external soft out",
       CLEAR_STR
       BGP_STR
       "Clear all external peers\n"
       "Soft reconfig\n"
       "Soft reconfig outbound update\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_external,
			BGP_CLEAR_SOFT_OUT, NULL);
}