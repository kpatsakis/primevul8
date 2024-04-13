cluster_parse (struct in_addr * pnt, int length)
{
  struct cluster_list tmp;
  struct cluster_list *cluster;

  tmp.length = length;
  tmp.list = pnt;

  cluster = hash_get (cluster_hash, &tmp, cluster_hash_alloc);
  cluster->refcnt++;
  return cluster;
}