bgp_attr_flush (struct attr *attr)
{
  if (attr->aspath && ! attr->aspath->refcnt)
    aspath_free (attr->aspath);
  if (attr->community && ! attr->community->refcnt)
    community_free (attr->community);
  if (attr->extra)
    {
      struct attr_extra *attre = attr->extra;

      if (attre->ecommunity && ! attre->ecommunity->refcnt)
        ecommunity_free (&attre->ecommunity);
      if (attre->cluster && ! attre->cluster->refcnt)
        cluster_free (attre->cluster);
      if (attre->transit && ! attre->transit->refcnt)
        transit_free (attre->transit);
    }
}