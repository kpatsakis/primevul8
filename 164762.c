connection_edge_process_inbuf(edge_connection_t *conn, int package_partial)
{
  tor_assert(conn);

  switch (conn->base_.state) {
    case AP_CONN_STATE_SOCKS_WAIT:
      if (connection_ap_handshake_process_socks(EDGE_TO_ENTRY_CONN(conn)) <0) {
        /* already marked */
        return -1;
      }
      return 0;
    case AP_CONN_STATE_NATD_WAIT:
      if (connection_ap_process_natd(EDGE_TO_ENTRY_CONN(conn)) < 0) {
        /* already marked */
        return -1;
      }
      return 0;
    case AP_CONN_STATE_HTTP_CONNECT_WAIT:
      if (connection_ap_process_http_connect(EDGE_TO_ENTRY_CONN(conn)) < 0) {
        return -1;
      }
      return 0;
    case AP_CONN_STATE_OPEN:
      if (! conn->base_.linked) {
        note_user_activity(approx_time());
      }

      FALLTHROUGH;
    case EXIT_CONN_STATE_OPEN:
      if (connection_edge_package_raw_inbuf(conn, package_partial, NULL) < 0) {
        /* (We already sent an end cell if possible) */
        connection_mark_for_close(TO_CONN(conn));
        return -1;
      }
      return 0;
    case AP_CONN_STATE_CONNECT_WAIT:
      if (connection_ap_supports_optimistic_data(EDGE_TO_ENTRY_CONN(conn))) {
        log_info(LD_EDGE,
                 "data from edge while in '%s' state. Sending it anyway. "
                 "package_partial=%d, buflen=%ld",
                 conn_state_to_string(conn->base_.type, conn->base_.state),
                 package_partial,
                 (long)connection_get_inbuf_len(TO_CONN(conn)));
        if (connection_edge_package_raw_inbuf(conn, package_partial, NULL)<0) {
          /* (We already sent an end cell if possible) */
          connection_mark_for_close(TO_CONN(conn));
          return -1;
        }
        return 0;
      }
      /* Fall through if the connection is on a circuit without optimistic
       * data support. */
      FALLTHROUGH;
    case EXIT_CONN_STATE_CONNECTING:
    case AP_CONN_STATE_RENDDESC_WAIT:
    case AP_CONN_STATE_CIRCUIT_WAIT:
    case AP_CONN_STATE_RESOLVE_WAIT:
    case AP_CONN_STATE_CONTROLLER_WAIT:
      log_info(LD_EDGE,
               "data from edge while in '%s' state. Leaving it on buffer.",
               conn_state_to_string(conn->base_.type, conn->base_.state));
      return 0;
  }
  log_warn(LD_BUG,"Got unexpected state %d. Closing.",conn->base_.state);
  tor_fragile_assert();
  connection_edge_end(conn, END_STREAM_REASON_INTERNAL);
  connection_mark_for_close(TO_CONN(conn));
  return -1;
}