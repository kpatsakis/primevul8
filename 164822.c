half_streams_get_total_allocation(void)
{
  return n_half_conns_allocated * sizeof(half_edge_t);
}