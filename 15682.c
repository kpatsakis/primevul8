DEFUN (clear_ip_bgp_as_vpnv4_soft,
       clear_ip_bgp_as_vpnv4_soft_cmd,
       "clear ip bgp <1-65535> vpnv4 unicast soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear peers with the AS number\n"
       "Address family\n"
       "Address Family Modifier\n"
       "Soft reconfig\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MPLS_VPN, clear_as,
			BGP_CLEAR_SOFT_BOTH, argv[0]);
}