DEFUN (show_ip_bgp_vpnv4_rd_summary,
       show_ip_bgp_vpnv4_rd_summary_cmd,
       "show ip bgp vpnv4 rd ASN:nn_or_IP-address:nn summary",
       SHOW_STR
       IP_STR
       BGP_STR
       "Display VPNv4 NLRI specific information\n"
       "Display information for a route distinguisher\n"
       "VPN Route Distinguisher\n"
       "Summary of BGP neighbor status\n")
{
  int ret;
  struct prefix_rd prd;

  ret = str2prefix_rd (argv[0], &prd);
  if (! ret)
    {
      vty_out (vty, "%% Malformed Route Distinguisher%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  return bgp_show_summary_vty (vty, NULL, AFI_IP, SAFI_MPLS_VPN);
}