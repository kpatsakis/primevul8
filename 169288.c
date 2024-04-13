transit_finish (void)
{
  hash_free (transit_hash);
  transit_hash = NULL;
}