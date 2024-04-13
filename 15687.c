bgp_open_capability_orf (struct stream *s, struct peer *peer,
                         afi_t afi, safi_t safi, u_char code)
{
  u_char cap_len;
  u_char orf_len;
  unsigned long capp;
  unsigned long orfp;
  unsigned long numberp;
  int number_of_orfs = 0;

  if (safi == SAFI_MPLS_VPN)
    safi = BGP_SAFI_VPNV4;

  stream_putc (s, BGP_OPEN_OPT_CAP);
  capp = stream_get_endp (s);           /* Set Capability Len Pointer */
  stream_putc (s, 0);                   /* Capability Length */
  stream_putc (s, code);                /* Capability Code */
  orfp = stream_get_endp (s);           /* Set ORF Len Pointer */
  stream_putc (s, 0);                   /* ORF Length */
  stream_putw (s, afi);
  stream_putc (s, 0);
  stream_putc (s, safi);
  numberp = stream_get_endp (s);        /* Set Number Pointer */
  stream_putc (s, 0);                   /* Number of ORFs */

  /* Address Prefix ORF */
  if (CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_ORF_PREFIX_SM)
      || CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_ORF_PREFIX_RM))
    {
      stream_putc (s, (code == CAPABILITY_CODE_ORF ?
		   ORF_TYPE_PREFIX : ORF_TYPE_PREFIX_OLD));

      if (CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_ORF_PREFIX_SM)
	  && CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_ORF_PREFIX_RM))
	{
	  SET_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_ADV);
	  SET_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_ADV);
	  stream_putc (s, ORF_MODE_BOTH);
	}
      else if (CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_ORF_PREFIX_SM))
	{
	  SET_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_ADV);
	  stream_putc (s, ORF_MODE_SEND);
	}
      else
	{
	  SET_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_ADV);
	  stream_putc (s, ORF_MODE_RECEIVE);
	}
      number_of_orfs++;
    }

  /* Total Number of ORFs. */
  stream_putc_at (s, numberp, number_of_orfs);

  /* Total ORF Len. */
  orf_len = stream_get_endp (s) - orfp - 1;
  stream_putc_at (s, orfp, orf_len);

  /* Total Capability Len. */
  cap_len = stream_get_endp (s) - capp - 1;
  stream_putc_at (s, capp, cap_len);
}