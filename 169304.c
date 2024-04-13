attrhash_cmp (const void *p1, const void *p2)
{
  const struct attr * attr1 = p1;
  const struct attr * attr2 = p2;

  if (attr1->flag == attr2->flag
      && attr1->origin == attr2->origin
      && attr1->nexthop.s_addr == attr2->nexthop.s_addr
      && attr1->aspath == attr2->aspath
      && attr1->community == attr2->community
      && attr1->med == attr2->med
      && attr1->local_pref == attr2->local_pref)
    {
      const struct attr_extra *ae1 = attr1->extra;
      const struct attr_extra *ae2 = attr2->extra;
      
      if (ae1 && ae2
          && ae1->aggregator_as == ae2->aggregator_as
          && ae1->aggregator_addr.s_addr == ae2->aggregator_addr.s_addr
          && ae1->weight == ae2->weight
#ifdef HAVE_IPV6
          && ae1->mp_nexthop_len == ae2->mp_nexthop_len
          && IPV6_ADDR_SAME (&ae1->mp_nexthop_global, &ae2->mp_nexthop_global)
          && IPV6_ADDR_SAME (&ae1->mp_nexthop_local, &ae2->mp_nexthop_local)
#endif /* HAVE_IPV6 */
          && IPV4_ADDR_SAME (&ae1->mp_nexthop_global_in, &ae2->mp_nexthop_global_in)
          && ae1->ecommunity == ae2->ecommunity
          && ae1->cluster == ae2->cluster
          && ae1->transit == ae2->transit)
        return 1;
      else if (ae1 || ae2)
        return 0;
      /* neither attribute has extra attributes, so they're same */
      return 1;
    }
  else
    return 0;
}