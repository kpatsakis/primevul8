bgp_attr_intern (struct attr *attr)
{
  struct attr *find;

  /* Intern referenced strucutre. */
  if (attr->aspath)
    {
      if (! attr->aspath->refcnt)
	attr->aspath = aspath_intern (attr->aspath);
      else
	attr->aspath->refcnt++;
    }
  if (attr->community)
    {
      if (! attr->community->refcnt)
	attr->community = community_intern (attr->community);
      else
	attr->community->refcnt++;
    }
  if (attr->extra)
    {
      struct attr_extra *attre = attr->extra;
      
      if (attre->ecommunity)
        {
          if (! attre->ecommunity->refcnt)
            attre->ecommunity = ecommunity_intern (attre->ecommunity);
          else
            attre->ecommunity->refcnt++;
          
        }
      if (attre->cluster)
        {
          if (! attre->cluster->refcnt)
            attre->cluster = cluster_intern (attre->cluster);
          else
            attre->cluster->refcnt++;
        }
      if (attre->transit)
        {
          if (! attre->transit->refcnt)
            attre->transit = transit_intern (attre->transit);
          else
            attre->transit->refcnt++;
        }
    }
  
  find = (struct attr *) hash_get (attrhash, attr, bgp_attr_hash_alloc);
  find->refcnt++;
  
  return find;
}