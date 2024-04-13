bgp_attr_aggregate_intern (struct bgp *bgp, u_char origin,
			   struct aspath *aspath,
			   struct community *community, int as_set)
{
  struct attr attr;
  struct attr *new;
  struct attr_extra *attre;

  memset (&attr, 0, sizeof (struct attr));
  attre = bgp_attr_extra_get (&attr);
  
  /* Origin attribute. */
  attr.origin = origin;
  attr.flag |= ATTR_FLAG_BIT (BGP_ATTR_ORIGIN);

  /* AS path attribute. */
  if (aspath)
    attr.aspath = aspath_intern (aspath);
  else
    attr.aspath = aspath_empty ();
  attr.flag |= ATTR_FLAG_BIT (BGP_ATTR_AS_PATH);

  /* Next hop attribute.  */
  attr.flag |= ATTR_FLAG_BIT (BGP_ATTR_NEXT_HOP);

  if (community)
    {
      attr.community = community;
      attr.flag |= ATTR_FLAG_BIT (BGP_ATTR_COMMUNITIES);
    }

  attre->weight = BGP_ATTR_DEFAULT_WEIGHT;
#ifdef HAVE_IPV6
  attre->mp_nexthop_len = IPV6_MAX_BYTELEN;
#endif
  if (! as_set)
    attr.flag |= ATTR_FLAG_BIT (BGP_ATTR_ATOMIC_AGGREGATE);
  attr.flag |= ATTR_FLAG_BIT (BGP_ATTR_AGGREGATOR);
  if (CHECK_FLAG (bgp->config, BGP_CONFIG_CONFEDERATION))
    attre->aggregator_as = bgp->confed_id;
  else
    attre->aggregator_as = bgp->as;
  attre->aggregator_addr = bgp->router_id;

  new = bgp_attr_intern (&attr);
  bgp_attr_extra_free (&attr);
  
  aspath_unintern (&new->aspath);
  return new;
}