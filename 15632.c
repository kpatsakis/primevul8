bgp_open_capability (struct stream *s, struct peer *peer)
{
  u_char len;
  unsigned long cp;
  afi_t afi;
  safi_t safi;

  /* Remember current pointer for Opt Parm Len. */
  cp = stream_get_endp (s);

  /* Opt Parm Len. */
  stream_putc (s, 0);

  /* Do not send capability. */
  if (! CHECK_FLAG (peer->sflags, PEER_STATUS_CAPABILITY_OPEN) 
      || CHECK_FLAG (peer->flags, PEER_FLAG_DONT_CAPABILITY))
    return;

  /* IPv4 unicast. */
  if (peer->afc[AFI_IP][SAFI_UNICAST])
    {
      peer->afc_adv[AFI_IP][SAFI_UNICAST] = 1;
      stream_putc (s, BGP_OPEN_OPT_CAP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN + 2);
      stream_putc (s, CAPABILITY_CODE_MP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN);
      stream_putw (s, AFI_IP);
      stream_putc (s, 0);
      stream_putc (s, SAFI_UNICAST);
    }
  /* IPv4 multicast. */
  if (peer->afc[AFI_IP][SAFI_MULTICAST])
    {
      peer->afc_adv[AFI_IP][SAFI_MULTICAST] = 1;
      stream_putc (s, BGP_OPEN_OPT_CAP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN + 2);
      stream_putc (s, CAPABILITY_CODE_MP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN);
      stream_putw (s, AFI_IP);
      stream_putc (s, 0);
      stream_putc (s, SAFI_MULTICAST);
    }
  /* IPv4 VPN */
  if (peer->afc[AFI_IP][SAFI_MPLS_VPN])
    {
      peer->afc_adv[AFI_IP][SAFI_MPLS_VPN] = 1;
      stream_putc (s, BGP_OPEN_OPT_CAP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN + 2);
      stream_putc (s, CAPABILITY_CODE_MP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN);
      stream_putw (s, AFI_IP);
      stream_putc (s, 0);
      stream_putc (s, BGP_SAFI_VPNV4);
    }
#ifdef HAVE_IPV6
  /* IPv6 unicast. */
  if (peer->afc[AFI_IP6][SAFI_UNICAST])
    {
      peer->afc_adv[AFI_IP6][SAFI_UNICAST] = 1;
      stream_putc (s, BGP_OPEN_OPT_CAP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN + 2);
      stream_putc (s, CAPABILITY_CODE_MP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN);
      stream_putw (s, AFI_IP6);
      stream_putc (s, 0);
      stream_putc (s, SAFI_UNICAST);
    }
  /* IPv6 multicast. */
  if (peer->afc[AFI_IP6][SAFI_MULTICAST])
    {
      peer->afc_adv[AFI_IP6][SAFI_MULTICAST] = 1;
      stream_putc (s, BGP_OPEN_OPT_CAP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN + 2);
      stream_putc (s, CAPABILITY_CODE_MP);
      stream_putc (s, CAPABILITY_CODE_MP_LEN);
      stream_putw (s, AFI_IP6);
      stream_putc (s, 0);
      stream_putc (s, SAFI_MULTICAST);
    }
#endif /* HAVE_IPV6 */

  /* Route refresh. */
  SET_FLAG (peer->cap, PEER_CAP_REFRESH_ADV);
  stream_putc (s, BGP_OPEN_OPT_CAP);
  stream_putc (s, CAPABILITY_CODE_REFRESH_LEN + 2);
  stream_putc (s, CAPABILITY_CODE_REFRESH_OLD);
  stream_putc (s, CAPABILITY_CODE_REFRESH_LEN);
  stream_putc (s, BGP_OPEN_OPT_CAP);
  stream_putc (s, CAPABILITY_CODE_REFRESH_LEN + 2);
  stream_putc (s, CAPABILITY_CODE_REFRESH);
  stream_putc (s, CAPABILITY_CODE_REFRESH_LEN);

  /* ORF capability. */
  for (afi = AFI_IP ; afi < AFI_MAX ; afi++)
    for (safi = SAFI_UNICAST ; safi < SAFI_MAX ; safi++)
      if (CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_ORF_PREFIX_SM)
	  || CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_ORF_PREFIX_RM))
	{
	  bgp_open_capability_orf (s, peer, afi, safi, CAPABILITY_CODE_ORF_OLD);
	  bgp_open_capability_orf (s, peer, afi, safi, CAPABILITY_CODE_ORF);
	}

  /* Dynamic capability. */
  if (CHECK_FLAG (peer->flags, PEER_FLAG_DYNAMIC_CAPABILITY))
    {
      SET_FLAG (peer->cap, PEER_CAP_DYNAMIC_ADV);
      stream_putc (s, BGP_OPEN_OPT_CAP);
      stream_putc (s, CAPABILITY_CODE_DYNAMIC_LEN + 2);
      stream_putc (s, CAPABILITY_CODE_DYNAMIC);
      stream_putc (s, CAPABILITY_CODE_DYNAMIC_LEN);
    }

  /* Graceful restart capability */
  if (bgp_flag_check (peer->bgp, BGP_FLAG_GRACEFUL_RESTART))
    {
      SET_FLAG (peer->cap, PEER_CAP_RESTART_ADV);
      stream_putc (s, BGP_OPEN_OPT_CAP);
      stream_putc (s, CAPABILITY_CODE_RESTART_LEN + 2);
      stream_putc (s, CAPABILITY_CODE_RESTART);
      stream_putc (s, CAPABILITY_CODE_RESTART_LEN);
      stream_putw (s, peer->bgp->restart_time);
     }

  /* Total Opt Parm Len. */
  len = stream_get_endp (s) - cp - 1;
  stream_putc_at (s, cp, len);
}