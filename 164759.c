connection_ap_mark_as_pending_circuit_(entry_connection_t *entry_conn,
                                       const char *fname, int lineno)
{
  connection_t *conn = ENTRY_TO_CONN(entry_conn);
  tor_assert(conn->state == AP_CONN_STATE_CIRCUIT_WAIT);
  tor_assert(conn->magic == ENTRY_CONNECTION_MAGIC);
  if (conn->marked_for_close)
    return;

  if (PREDICT_UNLIKELY(NULL == pending_entry_connections)) {
    pending_entry_connections = smartlist_new();
  }
  if (PREDICT_UNLIKELY(NULL == attach_pending_entry_connections_ev)) {
    attach_pending_entry_connections_ev = mainloop_event_postloop_new(
                                  attach_pending_entry_connections_cb, NULL);
  }
  if (PREDICT_UNLIKELY(smartlist_contains(pending_entry_connections,
                                          entry_conn))) {
    log_warn(LD_BUG, "What?? pending_entry_connections already contains %p! "
             "(Called from %s:%d.)",
             entry_conn, fname, lineno);
#ifdef DEBUGGING_17659
    const char *f2 = entry_conn->marked_pending_circ_file;
    log_warn(LD_BUG, "(Previously called from %s:%d.)\n",
             f2 ? f2 : "<NULL>",
             entry_conn->marked_pending_circ_line);
#endif /* defined(DEBUGGING_17659) */
    log_backtrace(LOG_WARN, LD_BUG, "To debug, this may help");
    return;
  }

#ifdef DEBUGGING_17659
  entry_conn->marked_pending_circ_line = (uint16_t) lineno;
  entry_conn->marked_pending_circ_file = fname;
#endif

  untried_pending_connections = 1;
  smartlist_add(pending_entry_connections, entry_conn);

  mainloop_event_activate(attach_pending_entry_connections_ev);
}