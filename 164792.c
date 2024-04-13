connection_edge_destroy(circid_t circ_id, edge_connection_t *conn)
{
  if (!conn->base_.marked_for_close) {
    log_info(LD_EDGE, "CircID %u: At an edge. Marking connection for close.",
             (unsigned) circ_id);
    if (conn->base_.type == CONN_TYPE_AP) {
      entry_connection_t *entry_conn = EDGE_TO_ENTRY_CONN(conn);
      connection_mark_unattached_ap(entry_conn, END_STREAM_REASON_DESTROY);
      control_event_stream_bandwidth(conn);
      control_event_stream_status(entry_conn, STREAM_EVENT_CLOSED,
                                  END_STREAM_REASON_DESTROY);
      conn->end_reason |= END_STREAM_REASON_FLAG_ALREADY_SENT_CLOSED;
    } else {
      /* closing the circuit, nothing to send an END to */
      conn->edge_has_sent_end = 1;
      conn->end_reason = END_STREAM_REASON_DESTROY;
      conn->end_reason |= END_STREAM_REASON_FLAG_ALREADY_SENT_CLOSED;
      connection_mark_and_flush(TO_CONN(conn));
    }
  }
  conn->cpath_layer = NULL;
  conn->on_circuit = NULL;
  return 0;
}