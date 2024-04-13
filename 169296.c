cluster_hash_alloc (void *p)
{
  struct cluster_list * val = (struct cluster_list *) p;
  struct cluster_list *cluster;

  cluster = XMALLOC (MTYPE_CLUSTER, sizeof (struct cluster_list));
  cluster->length = val->length;

  if (cluster->length)
    {
      cluster->list = XMALLOC (MTYPE_CLUSTER_VAL, val->length);
      memcpy (cluster->list, val->list, val->length);
    }
  else
    cluster->list = NULL;

  cluster->refcnt = 0;

  return cluster;
}