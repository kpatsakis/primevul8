bgp_afi_safi_valid_indices (afi_t afi, safi_t *safi)
{
  /* VPNvX are AFI specific */
  if ((afi == AFI_IP6 && *safi == BGP_SAFI_VPNV4)
      || (afi == AFI_IP && *safi == BGP_SAFI_VPNV6))
    {
      zlog_warn ("Invalid afi/safi combination (%u/%u)", afi, *safi);
      return 0;
    }
  
  switch (afi)
    {
      case AFI_IP:
#ifdef HAVE_IPV6
      case AFI_IP6:
#endif
        switch (*safi)
          {
            /* BGP VPNvX SAFI isn't contigious with others, remap */
            case BGP_SAFI_VPNV4:
            case BGP_SAFI_VPNV6:
              *safi = SAFI_MPLS_VPN;
            case SAFI_UNICAST:
            case SAFI_MULTICAST:
            case SAFI_MPLS_VPN:
              return 1;
          }
    }
  zlog_debug ("unknown afi/safi (%u/%u)", afi, *safi);
  
  return 0;
}