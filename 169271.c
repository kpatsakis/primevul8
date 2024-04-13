transit_init (void)
{
  transit_hash = hash_create (transit_hash_key_make, transit_hash_cmp);
}