bgp_dump_routes_attr (struct stream *s, struct attr *attr, 
                      struct prefix *prefix)
{
  unsigned long cp;
  unsigned long len;
  size_t aspath_lenp;
  struct aspath *aspath;

  /* Remember current pointer. */
  cp = stream_get_endp (s);

  /* Place holder of length. */
  stream_putw (s, 0);

  /* Origin attribute. */
  stream_putc (s, BGP_ATTR_FLAG_TRANS);
  stream_putc (s, BGP_ATTR_ORIGIN);
  stream_putc (s, 1);
  stream_putc (s, attr->origin);

  aspath = attr->aspath;
  
  stream_putc (s, BGP_ATTR_FLAG_TRANS|BGP_ATTR_FLAG_EXTLEN);
  stream_putc (s, BGP_ATTR_AS_PATH);
  aspath_lenp = stream_get_endp (s);
  stream_putw (s, 0);
  
  stream_putw_at (s, aspath_lenp, aspath_put (s, aspath, 1));

  /* Nexthop attribute. */
  /* If it's an IPv6 prefix, don't dump the IPv4 nexthop to save space */
  if(prefix != NULL
#ifdef HAVE_IPV6
     && prefix->family != AF_INET6
#endif /* HAVE_IPV6 */
     )
    {
      stream_putc (s, BGP_ATTR_FLAG_TRANS);
      stream_putc (s, BGP_ATTR_NEXT_HOP);
      stream_putc (s, 4);
      stream_put_ipv4 (s, attr->nexthop.s_addr);
    }

  /* MED attribute. */
  if (attr->flag & ATTR_FLAG_BIT (BGP_ATTR_MULTI_EXIT_DISC))
    {
      stream_putc (s, BGP_ATTR_FLAG_OPTIONAL);
      stream_putc (s, BGP_ATTR_MULTI_EXIT_DISC);
      stream_putc (s, 4);
      stream_putl (s, attr->med);
    }

  /* Local preference. */
  if (attr->flag & ATTR_FLAG_BIT (BGP_ATTR_LOCAL_PREF))
    {
      stream_putc (s, BGP_ATTR_FLAG_TRANS);
      stream_putc (s, BGP_ATTR_LOCAL_PREF);
      stream_putc (s, 4);
      stream_putl (s, attr->local_pref);
    }

  /* Atomic aggregate. */
  if (attr->flag & ATTR_FLAG_BIT (BGP_ATTR_ATOMIC_AGGREGATE))
    {
      stream_putc (s, BGP_ATTR_FLAG_TRANS);
      stream_putc (s, BGP_ATTR_ATOMIC_AGGREGATE);
      stream_putc (s, 0);
    }

  /* Aggregator. */
  if (attr->flag & ATTR_FLAG_BIT (BGP_ATTR_AGGREGATOR))
    {
      assert (attr->extra);
      stream_putc (s, BGP_ATTR_FLAG_OPTIONAL|BGP_ATTR_FLAG_TRANS);
      stream_putc (s, BGP_ATTR_AGGREGATOR);
      stream_putc (s, 8);
      stream_putl (s, attr->extra->aggregator_as);
      stream_put_ipv4 (s, attr->extra->aggregator_addr.s_addr);
    }

  /* Community attribute. */
  if (attr->flag & ATTR_FLAG_BIT (BGP_ATTR_COMMUNITIES))
    {
      if (attr->community->size * 4 > 255)
	{
	  stream_putc (s, BGP_ATTR_FLAG_OPTIONAL|BGP_ATTR_FLAG_TRANS|BGP_ATTR_FLAG_EXTLEN);
	  stream_putc (s, BGP_ATTR_COMMUNITIES);
	  stream_putw (s, attr->community->size * 4);
	}
      else
	{
	  stream_putc (s, BGP_ATTR_FLAG_OPTIONAL|BGP_ATTR_FLAG_TRANS);
	  stream_putc (s, BGP_ATTR_COMMUNITIES);
	  stream_putc (s, attr->community->size * 4);
	}
      stream_put (s, attr->community->val, attr->community->size * 4);
    }

#ifdef HAVE_IPV6
  /* Add a MP_NLRI attribute to dump the IPv6 next hop */
  if (prefix != NULL && prefix->family == AF_INET6 && attr->extra &&
     (attr->extra->mp_nexthop_len == 16 || attr->extra->mp_nexthop_len == 32) )
    {
      int sizep;
      struct attr_extra *attre = attr->extra;
      
      stream_putc(s, BGP_ATTR_FLAG_OPTIONAL);
      stream_putc(s, BGP_ATTR_MP_REACH_NLRI);
      sizep = stream_get_endp (s);

      /* MP header */
      stream_putc (s, 0);		/* Marker: Attribute length. */
      stream_putw(s, AFI_IP6);		/* AFI */
      stream_putc(s, SAFI_UNICAST);	/* SAFI */

      /* Next hop */
      stream_putc(s, attre->mp_nexthop_len);
      stream_put(s, &attre->mp_nexthop_global, 16);
      if (attre->mp_nexthop_len == 32)
        stream_put(s, &attre->mp_nexthop_local, 16);

      /* SNPA */
      stream_putc(s, 0);

      /* Prefix */
      stream_put_prefix(s, prefix);

      /* Set MP attribute length. */
      stream_putc_at (s, sizep, (stream_get_endp (s) - sizep) - 1);
    }
#endif /* HAVE_IPV6 */

  /* Return total size of attribute. */
  len = stream_get_endp (s) - cp - 2;
  stream_putw_at (s, cp, len);
}