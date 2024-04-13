bgp_attr_cluster_list (struct bgp_attr_parser_args *args)
{
  struct peer *const peer = args->peer; 
  struct attr *const attr = args->attr;
  const bgp_size_t length = args->length;
  
  /* Check length. */
  if (length % 4)
    {
      zlog (peer->log, LOG_ERR, "Bad cluster list length %d", length);

      return bgp_attr_malformed (args, BGP_NOTIFY_UPDATE_ATTR_LENG_ERR,
                                 args->total);
    }

  (bgp_attr_extra_get (attr))->cluster 
    = cluster_parse ((struct in_addr *)stream_pnt (peer->ibuf), length);
  
  /* XXX: Fix cluster_parse to use stream API and then remove this */
  stream_forward_getp (peer->ibuf, length);

  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_CLUSTER_LIST);

  return BGP_ATTR_PARSE_PROCEED;
}