DEFUN (clear_ip_bgp_all_vpnv4_soft,
       clear_ip_bgp_all_vpnv4_soft_cmd,
       "clear ip bgp * vpnv4 unicast soft",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all peers\n"
       "Address family\n"
       "Address Family Modifier\n"
       "Soft reconfig\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MPLS_VPN, clear_all,
			BGP_CLEAR_SOFT_BOTH, argv[0]);
}