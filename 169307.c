bgp_attr_nexthop (struct bgp_attr_parser_args *args)
{
  struct peer *const peer = args->peer; 
  struct attr *const attr = args->attr;
  const bgp_size_t length = args->length;
  
  in_addr_t nexthop_h, nexthop_n;

  /* Check nexthop attribute length. */
  if (length != 4)
    {
      zlog (peer->log, LOG_ERR, "Nexthop attribute length isn't four [%d]",
	      length);

      return bgp_attr_malformed (args,
                                 BGP_NOTIFY_UPDATE_ATTR_LENG_ERR,
                                 args->total);
    }

  /* According to section 6.3 of RFC4271, syntactically incorrect NEXT_HOP
     attribute must result in a NOTIFICATION message (this is implemented below).
     At the same time, semantically incorrect NEXT_HOP is more likely to be just
     logged locally (this is implemented somewhere else). The UPDATE message
     gets ignored in any of these cases. */
  nexthop_n = stream_get_ipv4 (peer->ibuf);
  nexthop_h = ntohl (nexthop_n);
  if (IPV4_NET0 (nexthop_h) || IPV4_NET127 (nexthop_h) || IPV4_CLASS_DE (nexthop_h))
    {
      char buf[INET_ADDRSTRLEN];
      inet_ntop (AF_INET, &nexthop_h, buf, INET_ADDRSTRLEN);
      zlog (peer->log, LOG_ERR, "Martian nexthop %s", buf);
      return bgp_attr_malformed (args,
                                 BGP_NOTIFY_UPDATE_INVAL_NEXT_HOP,
                                 args->total);
    }

  attr->nexthop.s_addr = nexthop_n;
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_NEXT_HOP);

  return BGP_ATTR_PARSE_PROCEED;
}