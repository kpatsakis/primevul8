connection_exit_begin_resolve(cell_t *cell, or_circuit_t *circ)
{
  edge_connection_t *dummy_conn;
  relay_header_t rh;

  assert_circuit_ok(TO_CIRCUIT(circ));
  relay_header_unpack(&rh, cell->payload);
  if (rh.length > RELAY_PAYLOAD_SIZE)
    return -1;

  /* This 'dummy_conn' only exists to remember the stream ID
   * associated with the resolve request; and to make the
   * implementation of dns.c more uniform.  (We really only need to
   * remember the circuit, the stream ID, and the hostname to be
   * resolved; but if we didn't store them in a connection like this,
   * the housekeeping in dns.c would get way more complicated.)
   */
  dummy_conn = edge_connection_new(CONN_TYPE_EXIT, AF_INET);
  dummy_conn->stream_id = rh.stream_id;
  dummy_conn->base_.address = tor_strndup(
                                       (char*)cell->payload+RELAY_HEADER_SIZE,
                                       rh.length);
  dummy_conn->base_.port = 0;
  dummy_conn->base_.state = EXIT_CONN_STATE_RESOLVEFAILED;
  dummy_conn->base_.purpose = EXIT_PURPOSE_RESOLVE;

  dummy_conn->on_circuit = TO_CIRCUIT(circ);

  /* send it off to the gethostbyname farm */
  switch (dns_resolve(dummy_conn)) {
    case -1: /* Impossible to resolve; a resolved cell was sent. */
      /* Connection freed; don't touch it. */
      return 0;
    case 1: /* The result was cached; a resolved cell was sent. */
      if (!dummy_conn->base_.marked_for_close)
        connection_free_(TO_CONN(dummy_conn));
      return 0;
    case 0: /* resolve added to pending list */
      assert_circuit_ok(TO_CIRCUIT(circ));
      break;
  }
  return 0;
}