relay_send_end_cell_from_edge(streamid_t stream_id, circuit_t *circ,
                              uint8_t reason, crypt_path_t *cpath_layer)
{
  char payload[1];

  if (CIRCUIT_PURPOSE_IS_CLIENT(circ->purpose)) {
    /* Never send the server an informative reason code; it doesn't need to
     * know why the client stream is failing. */
    reason = END_STREAM_REASON_MISC;
  }

  payload[0] = (char) reason;

  /* Note: we have to use relay_send_command_from_edge here, not
   * connection_edge_end or connection_edge_send_command, since those require
   * that we have a stream connected to a circuit, and we don't connect to a
   * circuit until we have a pending/successful resolve. */
  return relay_send_command_from_edge(stream_id, circ, RELAY_COMMAND_END,
                                      payload, 1, cpath_layer);
}