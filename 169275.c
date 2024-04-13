cluster_hash_key_make (void *p)
{
  const struct cluster_list *cluster = p;

  return jhash(cluster->list, cluster->length, 0);
}