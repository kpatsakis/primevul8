connection_half_edge_is_valid_data(const smartlist_t *half_conns,
                                   streamid_t stream_id)
{
  half_edge_t *half = connection_half_edge_find_stream_id(half_conns,
                                                          stream_id);

  if (!half)
    return 0;

  if (half->data_pending > 0) {
    half->data_pending--;
    return 1;
  }

  return 0;
}