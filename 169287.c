transit_hash_cmp (const void *p1, const void *p2)
{
  const struct transit * transit1 = p1;
  const struct transit * transit2 = p2;

  return (transit1->length == transit2->length &&
	  memcmp (transit1->val, transit2->val, transit1->length) == 0);
}