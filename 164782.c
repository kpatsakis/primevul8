connection_ap_detach_retriable(entry_connection_t *conn,
                               origin_circuit_t *circ,
                               int reason)
{
  control_event_stream_status(conn, STREAM_EVENT_FAILED_RETRIABLE, reason);
  ENTRY_TO_CONN(conn)->timestamp_last_read_allowed = time(NULL);

  /* Roll back path bias use state so that we probe the circuit
   * if nothing else succeeds on it */
  pathbias_mark_use_rollback(circ);

  if (conn->pending_optimistic_data) {
    buf_set_to_copy(&conn->sending_optimistic_data,
                    conn->pending_optimistic_data);
  }

  if (!get_options()->LeaveStreamsUnattached || conn->use_begindir) {
    /* If we're attaching streams ourself, or if this connection is
     * a tunneled directory connection, then just attach it. */
    ENTRY_TO_CONN(conn)->state = AP_CONN_STATE_CIRCUIT_WAIT;
    circuit_detach_stream(TO_CIRCUIT(circ),ENTRY_TO_EDGE_CONN(conn));
    connection_ap_mark_as_pending_circuit(conn);
  } else {
    connection_entry_set_controller_wait(conn);
    circuit_detach_stream(TO_CIRCUIT(circ),ENTRY_TO_EDGE_CONN(conn));
  }
  return 0;
}