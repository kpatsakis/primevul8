cluster_intern (struct cluster_list *cluster)
{
  struct cluster_list *find;

  find = hash_get (cluster_hash, cluster, cluster_hash_alloc);
  find->refcnt++;

  return find;
}