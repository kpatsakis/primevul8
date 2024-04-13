DEFUN (clear_ip_bgp_external,
       clear_ip_bgp_external_cmd,
       "clear ip bgp external",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all external peers\n")
{
  return bgp_clear_vty (vty, NULL, 0, 0, clear_external, BGP_CLEAR_SOFT_NONE, NULL);
}