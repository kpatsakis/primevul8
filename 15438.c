DEFUN (clear_bgp_as_soft_in,
       clear_bgp_as_soft_in_cmd,
       "clear bgp <1-65535> soft in",
       CLEAR_STR
       BGP_STR
       "Clear peers with the AS number\n"
       "Soft reconfig\n"
       "Soft reconfig inbound update\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP6, SAFI_UNICAST, clear_as,
			BGP_CLEAR_SOFT_IN, argv[0]);
}