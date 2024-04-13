bgp_packet_withdraw (struct peer *peer, struct stream *s, struct prefix *p,
		     afi_t afi, safi_t safi, struct prefix_rd *prd,
		     u_char *tag)
{
  unsigned long cp;
  unsigned long attrlen_pnt;
  bgp_size_t size;

  cp = stream_get_endp (s);

  stream_putc (s, BGP_ATTR_FLAG_OPTIONAL);
  stream_putc (s, BGP_ATTR_MP_UNREACH_NLRI);

  attrlen_pnt = stream_get_endp (s);
  stream_putc (s, 0);		/* Length of this attribute. */

  stream_putw (s, family2afi (p->family));

  if (safi == SAFI_MPLS_VPN)
    {
      /* SAFI */
      stream_putc (s, SAFI_MPLS_LABELED_VPN);

      /* prefix. */
      stream_putc (s, p->prefixlen + 88);
      stream_put (s, tag, 3);
      stream_put (s, prd->val, 8);
      stream_put (s, &p->u.prefix, PSIZE (p->prefixlen));
    }
  else
    {
      /* SAFI */
      stream_putc (s, safi);

      /* prefix */
      stream_put_prefix (s, p);
    }

  /* Set MP attribute length. */
  size = stream_get_endp (s) - attrlen_pnt - 1;
  stream_putc_at (s, attrlen_pnt, size);

  return stream_get_endp (s) - cp;
}