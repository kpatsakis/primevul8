bgp_attr_originator_id (struct bgp_attr_parser_args *args)
{
  struct peer *const peer = args->peer; 
  struct attr *const attr = args->attr;
  const bgp_size_t length = args->length;
  
  /* Length check. */
  if (length != 4)
    {
      zlog (peer->log, LOG_ERR, "Bad originator ID length %d", length);

      return bgp_attr_malformed (args,
                                 BGP_NOTIFY_UPDATE_ATTR_LENG_ERR,
                                 args->total);
    }

  (bgp_attr_extra_get (attr))->originator_id.s_addr 
    = stream_get_ipv4 (peer->ibuf);

  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_ORIGINATOR_ID);

  return BGP_ATTR_PARSE_PROCEED;
}