bgp_attr_aspath (struct bgp_attr_parser_args *args)
{
  struct attr *const attr = args->attr;
  struct peer *const peer = args->peer; 
  const bgp_size_t length = args->length;
  
  /*
   * peer with AS4 => will get 4Byte ASnums
   * otherwise, will get 16 Bit
   */
  attr->aspath = aspath_parse (peer->ibuf, length, 
                               CHECK_FLAG (peer->cap, PEER_CAP_AS4_RCV));

  /* In case of IBGP, length will be zero. */
  if (! attr->aspath)
    {
      zlog (peer->log, LOG_ERR,
            "Malformed AS path from %s, length is %d",
            peer->host, length);
      return bgp_attr_malformed (args, BGP_NOTIFY_UPDATE_MAL_AS_PATH, 0);
    }

  /* Set aspath attribute flag. */
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_AS_PATH);

  return BGP_ATTR_PARSE_PROCEED;
}