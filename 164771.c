connection_half_edge_is_valid_connected(const smartlist_t *half_conns,
                                        streamid_t stream_id)
{
  half_edge_t *half = connection_half_edge_find_stream_id(half_conns,
                                                          stream_id);

  if (!half)
    return 0;

  if (half->connected_pending) {
    half->connected_pending = 0;
    return 1;
  }

  return 0;
}