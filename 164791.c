connection_ap_attach_pending(int retry)
{
  if (PREDICT_UNLIKELY(!pending_entry_connections)) {
    return;
  }

  if (untried_pending_connections == 0 && !retry)
    return;

  /* Don't allow any modifications to list while we are iterating over
   * it.  We'll put streams back on this list if we can't attach them
   * immediately. */
  smartlist_t *pending = pending_entry_connections;
  pending_entry_connections = smartlist_new();

  SMARTLIST_FOREACH_BEGIN(pending,
                          entry_connection_t *, entry_conn) {
    connection_t *conn = ENTRY_TO_CONN(entry_conn);
    tor_assert(conn && entry_conn);
    if (conn->marked_for_close) {
      UNMARK();
      continue;
    }
    if (conn->magic != ENTRY_CONNECTION_MAGIC) {
      log_warn(LD_BUG, "%p has impossible magic value %u.",
               entry_conn, (unsigned)conn->magic);
      UNMARK();
      continue;
    }
    if (conn->state != AP_CONN_STATE_CIRCUIT_WAIT) {
      log_warn(LD_BUG, "%p is no longer in circuit_wait. Its current state "
               "is %s. Why is it on pending_entry_connections?",
               entry_conn,
               conn_state_to_string(conn->type, conn->state));
      UNMARK();
      continue;
    }

    /* Okay, we're through the sanity checks. Try to handle this stream. */
    if (connection_ap_handshake_attach_circuit(entry_conn) < 0) {
      if (!conn->marked_for_close)
        connection_mark_unattached_ap(entry_conn,
                                      END_STREAM_REASON_CANT_ATTACH);
    }

    if (! conn->marked_for_close &&
        conn->type == CONN_TYPE_AP &&
        conn->state == AP_CONN_STATE_CIRCUIT_WAIT) {
      /* Is it still waiting for a circuit? If so, we didn't attach it,
       * so it's still pending.  Put it back on the list.
       */
      if (!smartlist_contains(pending_entry_connections, entry_conn)) {
        smartlist_add(pending_entry_connections, entry_conn);
        continue;
      }
    }

    /* If we got here, then we either closed the connection, or
     * we attached it. */
    UNMARK();
  } SMARTLIST_FOREACH_END(entry_conn);

  smartlist_free(pending);
  untried_pending_connections = 0;
}