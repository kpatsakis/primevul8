DEFUN (clear_bgp_as_soft_out,
       clear_bgp_as_soft_out_cmd,
       "clear bgp <1-65535> soft out",
       CLEAR_STR
       BGP_STR
       "Clear peers with the AS number\n"
       "Soft reconfig\n"
       "Soft reconfig outbound update\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_as,
			BGP_CLEAR_SOFT_OUT, argv[0]);
}