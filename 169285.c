bgp_attr_local_pref (struct bgp_attr_parser_args *args)
{
  struct peer *const peer = args->peer; 
  struct attr *const attr = args->attr;
  const bgp_size_t length = args->length;
  
  /* Length check. */
  if (length != 4)
  {
    zlog (peer->log, LOG_ERR, "LOCAL_PREF attribute length isn't 4 [%u]",
          length);
    return bgp_attr_malformed (args,
                               BGP_NOTIFY_UPDATE_ATTR_LENG_ERR,
                               args->total);
  }

  /* If it is contained in an UPDATE message that is received from an
     external peer, then this attribute MUST be ignored by the
     receiving speaker. */
  if (peer_sort (peer) == BGP_PEER_EBGP)
    {
      stream_forward_getp (peer->ibuf, length);
      return BGP_ATTR_PARSE_PROCEED;
    }

  attr->local_pref = stream_getl (peer->ibuf);

  /* Set atomic aggregate flag. */
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_LOCAL_PREF);

  return BGP_ATTR_PARSE_PROCEED;
}