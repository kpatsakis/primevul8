bgp_attr_unintern_sub (struct attr *attr)
{
  /* aspath refcount shoud be decrement. */
  if (attr->aspath)
    aspath_unintern (&attr->aspath);
  UNSET_FLAG(attr->flag, BGP_ATTR_AS_PATH);
  
  if (attr->community)
    community_unintern (&attr->community);
  UNSET_FLAG(attr->flag, BGP_ATTR_COMMUNITIES);
  
  if (attr->extra)
    {
      if (attr->extra->ecommunity)
        ecommunity_unintern (&attr->extra->ecommunity);
      UNSET_FLAG(attr->flag, BGP_ATTR_EXT_COMMUNITIES);
      
      if (attr->extra->cluster)
        cluster_unintern (attr->extra->cluster);
      UNSET_FLAG(attr->flag, BGP_ATTR_CLUSTER_LIST);
      
      if (attr->extra->transit)
        transit_unintern (attr->extra->transit);
    }
}