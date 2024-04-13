bgp_afi_safi_valid_indices (afi_t afi, safi_t *safi)
{
  switch (afi)
    {
      case AFI_IP:
#ifdef HAVE_IPV6
      case AFI_IP6:
#endif
        switch (*safi)
          {
            /* BGP MPLS-labeled VPN SAFI isn't contigious with others, remap */
            case SAFI_MPLS_LABELED_VPN:
              *safi = SAFI_MPLS_VPN;
            case SAFI_UNICAST:
            case SAFI_MULTICAST:
              return 1;
          }
    }
  zlog_debug ("unknown afi/safi (%u/%u)", afi, *safi);
  
  return 0;
}