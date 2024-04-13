connection_half_edge_compare_bsearch(const void *key, const void **member)
{
  const half_edge_t *e2;
  tor_assert(key);
  tor_assert(member && *(half_edge_t**)member);
  e2 = *(const half_edge_t **)member;

  return *(const streamid_t*)key - e2->stream_id;
}