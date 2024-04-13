bgp_mp_unreach_parse (struct bgp_attr_parser_args *args,
		      struct bgp_nlri *mp_withdraw)
{
  struct stream *s;
  afi_t afi;
  safi_t safi;
  u_int16_t withdraw_len;
  int ret;
  struct peer *const peer = args->peer;  
  const bgp_size_t length = args->length;

  s = peer->ibuf;
  
#define BGP_MP_UNREACH_MIN_SIZE 3
  if ((length > STREAM_READABLE(s)) || (length <  BGP_MP_UNREACH_MIN_SIZE))
    return BGP_ATTR_PARSE_ERROR;
  
  afi = stream_getw (s);
  safi = stream_getc (s);
  
  withdraw_len = length - BGP_MP_UNREACH_MIN_SIZE;

  if (safi != SAFI_MPLS_LABELED_VPN)
    {
      ret = bgp_nlri_sanity_check (peer, afi, stream_pnt (s), withdraw_len);
      if (ret < 0)
	return BGP_ATTR_PARSE_ERROR;
    }

  mp_withdraw->afi = afi;
  mp_withdraw->safi = safi;
  mp_withdraw->nlri = stream_pnt (s);
  mp_withdraw->length = withdraw_len;

  stream_forward_getp (s, withdraw_len);

  return BGP_ATTR_PARSE_PROCEED;
}