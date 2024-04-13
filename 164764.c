connection_ap_rescan_and_attach_pending(void)
{
  entry_connection_t *entry_conn;
  smartlist_t *conns = get_connection_array();

  if (PREDICT_UNLIKELY(NULL == pending_entry_connections))
    pending_entry_connections = smartlist_new();

  SMARTLIST_FOREACH_BEGIN(conns, connection_t *, conn) {
    if (conn->marked_for_close ||
        conn->type != CONN_TYPE_AP ||
        conn->state != AP_CONN_STATE_CIRCUIT_WAIT)
      continue;

    entry_conn = TO_ENTRY_CONN(conn);
    tor_assert(entry_conn);
    if (! smartlist_contains(pending_entry_connections, entry_conn)) {
      log_warn(LD_BUG, "Found a connection %p that was supposed to be "
               "in pending_entry_connections, but wasn't. No worries; "
               "adding it.",
               pending_entry_connections);
      untried_pending_connections = 1;
      connection_ap_mark_as_pending_circuit(entry_conn);
    }

  } SMARTLIST_FOREACH_END(conn);

  connection_ap_attach_pending(1);
}