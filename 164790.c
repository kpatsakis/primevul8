connection_half_edge_is_valid_resolved(smartlist_t *half_conns,
                                       streamid_t stream_id)
{
  return connection_half_edge_is_valid_end(half_conns, stream_id);
}