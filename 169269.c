cluster_hash_cmp (const void *p1, const void *p2)
{
  const struct cluster_list * cluster1 = p1;
  const struct cluster_list * cluster2 = p2;

  return (cluster1->length == cluster2->length &&
	  memcmp (cluster1->list, cluster2->list, cluster1->length) == 0);
}