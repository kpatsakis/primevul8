bgp_attr_as4_aggregator (struct bgp_attr_parser_args *args,
		         as_t *as4_aggregator_as,
		         struct in_addr *as4_aggregator_addr)
{
  struct peer *const peer = args->peer; 
  struct attr *const attr = args->attr;
  const bgp_size_t length = args->length;
      
  if (length != 8)
    {
      zlog (peer->log, LOG_ERR, "New Aggregator length is not 8 [%d]",
            length);
      return bgp_attr_malformed (args,
                                 BGP_NOTIFY_UPDATE_ATTR_LENG_ERR,
                                 0);
    }
  
  *as4_aggregator_as = stream_getl (peer->ibuf);
  as4_aggregator_addr->s_addr = stream_get_ipv4 (peer->ibuf);

  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_AS4_AGGREGATOR);

  return BGP_ATTR_PARSE_PROCEED;
}