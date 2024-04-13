attrhash_init (void)
{
  attrhash = hash_create (attrhash_key_make, attrhash_cmp);
}