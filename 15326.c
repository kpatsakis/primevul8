DEFUN (clear_ip_bgp_as,
       clear_ip_bgp_as_cmd,
       "clear ip bgp <1-65535>",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear peers with the AS number\n")
{
  return bgp_clear_vty (vty, NULL, 0, 0, clear_as, BGP_CLEAR_SOFT_NONE, argv[0]);
}       