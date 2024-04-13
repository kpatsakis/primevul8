cluster_free (struct cluster_list *cluster)
{
  if (cluster->list)
    XFREE (MTYPE_CLUSTER_VAL, cluster->list);
  XFREE (MTYPE_CLUSTER, cluster);
}