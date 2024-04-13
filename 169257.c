attrhash_key_make (void *p)
{
  const struct attr * attr = (struct attr *) p;
  uint32_t key = 0;
#define MIX(val)	key = jhash_1word(val, key)

  MIX(attr->origin);
  MIX(attr->nexthop.s_addr);
  MIX(attr->med);
  MIX(attr->local_pref);

  key += attr->origin;
  key += attr->nexthop.s_addr;
  key += attr->med;
  key += attr->local_pref;
  
  if (attr->extra)
    {
      MIX(attr->extra->aggregator_as);
      MIX(attr->extra->aggregator_addr.s_addr);
      MIX(attr->extra->weight);
      MIX(attr->extra->mp_nexthop_global_in.s_addr);
    }
  
  if (attr->aspath)
    MIX(aspath_key_make (attr->aspath));
  if (attr->community)
    MIX(community_hash_make (attr->community));
  
  if (attr->extra)
    {
      if (attr->extra->ecommunity)
        MIX(ecommunity_hash_make (attr->extra->ecommunity));
      if (attr->extra->cluster)
        MIX(cluster_hash_key_make (attr->extra->cluster));
      if (attr->extra->transit)
        MIX(transit_hash_key_make (attr->extra->transit));

#ifdef HAVE_IPV6
      MIX(attr->extra->mp_nexthop_len);
      key = jhash(attr->extra->mp_nexthop_global.s6_addr, 16, key);
      key = jhash(attr->extra->mp_nexthop_local.s6_addr, 16, key);
#endif /* HAVE_IPV6 */
    }

  return key;
}