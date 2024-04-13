bgp_attr_atomic (struct bgp_attr_parser_args *args)
{
  struct peer *const peer = args->peer; 
  struct attr *const attr = args->attr;
  const bgp_size_t length = args->length;
  
  /* Length check. */
  if (length != 0)
    {
      zlog (peer->log, LOG_ERR, "ATOMIC_AGGREGATE attribute length isn't 0 [%u]",
            length);
      return bgp_attr_malformed (args,
                                 BGP_NOTIFY_UPDATE_ATTR_LENG_ERR,
                                 args->total);
    }

  /* Set atomic aggregate flag. */
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_ATOMIC_AGGREGATE);

  return BGP_ATTR_PARSE_PROCEED;
}