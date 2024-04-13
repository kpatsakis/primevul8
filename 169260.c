bgp_attr_check (struct peer *peer, struct attr *attr)
{
  u_char type = 0;
  
  if (! CHECK_FLAG (attr->flag, ATTR_FLAG_BIT (BGP_ATTR_ORIGIN)))
    type = BGP_ATTR_ORIGIN;

  if (! CHECK_FLAG (attr->flag, ATTR_FLAG_BIT (BGP_ATTR_AS_PATH)))
    type = BGP_ATTR_AS_PATH;

  if (! CHECK_FLAG (attr->flag, ATTR_FLAG_BIT (BGP_ATTR_NEXT_HOP)))
    type = BGP_ATTR_NEXT_HOP;

  if (peer_sort (peer) == BGP_PEER_IBGP
      && ! CHECK_FLAG (attr->flag, ATTR_FLAG_BIT (BGP_ATTR_LOCAL_PREF)))
    type = BGP_ATTR_LOCAL_PREF;

  if (type)
    {
      zlog (peer->log, LOG_WARNING, 
	    "%s Missing well-known attribute %d.",
	    peer->host, type);
      bgp_notify_send_with_data (peer, 
				 BGP_NOTIFY_UPDATE_ERR, 
				 BGP_NOTIFY_UPDATE_MISS_ATTR,
				 &type, 1);
      return BGP_ATTR_PARSE_ERROR;
    }
  return BGP_ATTR_PARSE_PROCEED;
}