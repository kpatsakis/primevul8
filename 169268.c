bgp_attr_ext_communities (struct bgp_attr_parser_args *args)
{
  struct peer *const peer = args->peer;  
  struct attr *const attr = args->attr;  
  const bgp_size_t length = args->length;
  
  if (length == 0)
    {
      if (attr->extra)
        attr->extra->ecommunity = NULL;
      /* Empty extcomm doesn't seem to be invalid per se */
      return BGP_ATTR_PARSE_PROCEED;
    }

  (bgp_attr_extra_get (attr))->ecommunity =
    ecommunity_parse ((u_int8_t *)stream_pnt (peer->ibuf), length);
  /* XXX: fix ecommunity_parse to use stream API */
  stream_forward_getp (peer->ibuf, length);
  
  if (!attr->extra->ecommunity)
    return bgp_attr_malformed (args,
                               BGP_NOTIFY_UPDATE_OPT_ATTR_ERR,
                               args->total);
  
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_EXT_COMMUNITIES);

  return BGP_ATTR_PARSE_PROCEED;
}