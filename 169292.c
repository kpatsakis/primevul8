bgp_attr_community (struct bgp_attr_parser_args *args)
{
  struct peer *const peer = args->peer; 
  struct attr *const attr = args->attr;  
  const bgp_size_t length = args->length;
  
  if (length == 0)
    {
      attr->community = NULL;
      return BGP_ATTR_PARSE_PROCEED;
    }
  
  attr->community =
    community_parse ((u_int32_t *)stream_pnt (peer->ibuf), length);
  
  /* XXX: fix community_parse to use stream API and remove this */
  stream_forward_getp (peer->ibuf, length);

  if (!attr->community)
    return bgp_attr_malformed (args,
                               BGP_NOTIFY_UPDATE_OPT_ATTR_ERR,
                               args->total);
  
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_COMMUNITIES);

  return BGP_ATTR_PARSE_PROCEED;
}