handle_hs_exit_conn(circuit_t *circ, edge_connection_t *conn)
{
  int ret;
  origin_circuit_t *origin_circ;

  assert_circuit_ok(circ);
  tor_assert(circ->purpose == CIRCUIT_PURPOSE_S_REND_JOINED);
  tor_assert(conn);

  log_debug(LD_REND, "Connecting the hidden service rendezvous circuit "
                     "to the service destination.");

  origin_circ = TO_ORIGIN_CIRCUIT(circ);
  conn->base_.address = tor_strdup("(rendezvous)");
  conn->base_.state = EXIT_CONN_STATE_CONNECTING;

  if (origin_circ->hs_ident) {
    /* Setup the identifier to be the one for the circuit service. */
    conn->hs_ident =
      hs_ident_edge_conn_new(&origin_circ->hs_ident->identity_pk);
    tor_assert(connection_edge_is_rendezvous_stream(conn));
    ret = hs_service_set_conn_addr_port(origin_circ, conn);
  } else {
    /* We should never get here if the circuit's purpose is rendezvous. */
    tor_assert_nonfatal_unreached();
    return -1;
  }
  if (ret < 0) {
    log_info(LD_REND, "Didn't find rendezvous service at %s",
             connection_describe_peer(TO_CONN(conn)));
    /* Send back reason DONE because we want to make hidden service port
     * scanning harder thus instead of returning that the exit policy
     * didn't match, which makes it obvious that the port is closed,
     * return DONE and kill the circuit. That way, a user (malicious or
     * not) needs one circuit per bad port unless it matches the policy of
     * the hidden service. */
    relay_send_end_cell_from_edge(conn->stream_id, circ,
                                  END_STREAM_REASON_DONE,
                                  origin_circ->cpath->prev);
    connection_free_(TO_CONN(conn));

    /* Drop the circuit here since it might be someone deliberately
     * scanning the hidden service ports. Note that this mitigates port
     * scanning by adding more work on the attacker side to successfully
     * scan but does not fully solve it. */
    if (ret < -1) {
      return END_CIRC_AT_ORIGIN;
    } else {
      return 0;
    }
  }

  /* Link the circuit and the connection crypt path. */
  conn->cpath_layer = origin_circ->cpath->prev;

  /* If this is the first stream on this circuit, tell circpad */
  if (!origin_circ->p_streams)
    circpad_machine_event_circ_has_streams(origin_circ);

  /* Add it into the linked list of p_streams on this circuit */
  conn->next_stream = origin_circ->p_streams;
  origin_circ->p_streams = conn;
  conn->on_circuit = circ;
  assert_circuit_ok(circ);

  hs_inc_rdv_stream_counter(origin_circ);

  /* If it's an onion service connection, we might want to include the proxy
   * protocol header: */
  if (conn->hs_ident) {
    hs_circuit_id_protocol_t circuit_id_protocol =
      hs_service_exports_circuit_id(&conn->hs_ident->identity_pk);
    export_hs_client_circuit_id(conn, circuit_id_protocol);
  }

  /* Connect tor to the hidden service destination. */
  connection_exit_connect(conn);

  /* For path bias: This circuit was used successfully */
  pathbias_mark_use_success(origin_circ);
  return 0;
}