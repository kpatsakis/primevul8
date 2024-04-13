cluster_finish (void)
{
  hash_free (cluster_hash);
  cluster_hash = NULL;
}