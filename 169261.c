cluster_unintern (struct cluster_list *cluster)
{
  if (cluster->refcnt)
    cluster->refcnt--;

  if (cluster->refcnt == 0)
    {
      hash_release (cluster_hash, cluster);
      cluster_free (cluster);
    }
}