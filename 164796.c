connection_half_edge_is_valid_end(smartlist_t *half_conns,
                                  streamid_t stream_id)
{
  half_edge_t *half;
  int found, remove_idx;

  if (!half_conns)
    return 0;

  remove_idx = smartlist_bsearch_idx(half_conns, &stream_id,
                                    connection_half_edge_compare_bsearch,
                                    &found);
  if (!found)
    return 0;

  half = smartlist_get(half_conns, remove_idx);
  smartlist_del_keeporder(half_conns, remove_idx);
  half_edge_free(half);
  return 1;
}