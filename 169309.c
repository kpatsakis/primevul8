attrhash_finish (void)
{
  hash_free (attrhash);
  attrhash = NULL;
}