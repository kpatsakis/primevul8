connection_half_edge_find_stream_id(const smartlist_t *half_conns,
                                    streamid_t stream_id)
{
  if (!half_conns)
    return NULL;

  return smartlist_bsearch(half_conns, &stream_id,
                           connection_half_edge_compare_bsearch);
}