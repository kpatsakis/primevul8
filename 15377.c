bgp_clear_vty_error (struct vty *vty, struct peer *peer, afi_t afi,
		     safi_t safi, int error)
{
  switch (error)
    {
    case BGP_ERR_AF_UNCONFIGURED:
      vty_out (vty,
	       "%%BGP: Enable %s %s address family for the neighbor %s%s",
	       afi == AFI_IP6 ? "IPv6" : safi == SAFI_MPLS_VPN ? "VPNv4" : "IPv4",
	       safi == SAFI_MULTICAST ? "Multicast" : "Unicast",
	       peer->host, VTY_NEWLINE);
      break;
    case BGP_ERR_SOFT_RECONFIG_UNCONFIGURED:
      vty_out (vty, "%%BGP: Inbound soft reconfig for %s not possible as it%s      has neither refresh capability, nor inbound soft reconfig%s", peer->host, VTY_NEWLINE, VTY_NEWLINE);
      break;
    default:
      break;
    }
}