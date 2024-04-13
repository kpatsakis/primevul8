DEFUN (clear_ip_bgp_all_vpnv4_soft_out,
       clear_ip_bgp_all_vpnv4_soft_out_cmd,
       "clear ip bgp * vpnv4 unicast soft out",
       CLEAR_STR
       IP_STR
       BGP_STR
       "Clear all peers\n"
       "Address family\n"
       "Address Family Modifier\n"
       "Soft reconfig\n"
       "Soft reconfig outbound update\n")
{
  return bgp_clear_vty (vty, NULL, AFI_IP, SAFI_MPLS_VPN, clear_all,
			BGP_CLEAR_SOFT_OUT, NULL);
}