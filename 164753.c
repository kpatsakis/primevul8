connection_half_edge_is_valid_sendme(const smartlist_t *half_conns,
                                     streamid_t stream_id)
{
  half_edge_t *half = connection_half_edge_find_stream_id(half_conns,
                                                          stream_id);

  if (!half)
    return 0;

  if (half->sendmes_pending > 0) {
    half->sendmes_pending--;
    return 1;
  }

  return 0;
}