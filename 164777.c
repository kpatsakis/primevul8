half_edge_free_(half_edge_t *he)
{
  if (!he)
    return;
  --n_half_conns_allocated;
  tor_free(he);
}