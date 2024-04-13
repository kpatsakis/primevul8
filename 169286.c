cluster_init (void)
{
  cluster_hash = hash_create (cluster_hash_key_make, cluster_hash_cmp);
}