bgp_attr_finish (void)
{
  aspath_finish ();
  attrhash_finish ();
  community_finish ();
  ecommunity_finish ();
  cluster_finish ();
  transit_finish ();
}