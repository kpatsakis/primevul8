bgp_attr_aggregator (struct bgp_attr_parser_args *args)
{
  struct peer *const peer = args->peer; 
  struct attr *const attr = args->attr;
  const bgp_size_t length = args->length;
  
  int wantedlen = 6;
  struct attr_extra *attre = bgp_attr_extra_get (attr);
  
  /* peer with AS4 will send 4 Byte AS, peer without will send 2 Byte */
  if (CHECK_FLAG (peer->cap, PEER_CAP_AS4_RCV))
    wantedlen = 8;
  
  if (length != wantedlen)
    {
      zlog (peer->log, LOG_ERR, "AGGREGATOR attribute length isn't %u [%u]",
            wantedlen, length);
      return bgp_attr_malformed (args,
                                 BGP_NOTIFY_UPDATE_ATTR_LENG_ERR,
                                 args->total);
    }
  
  if ( CHECK_FLAG (peer->cap, PEER_CAP_AS4_RCV ) )
    attre->aggregator_as = stream_getl (peer->ibuf);
  else
    attre->aggregator_as = stream_getw (peer->ibuf);
  attre->aggregator_addr.s_addr = stream_get_ipv4 (peer->ibuf);

  /* Set atomic aggregate flag. */
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_AGGREGATOR);

  return BGP_ATTR_PARSE_PROCEED;
}