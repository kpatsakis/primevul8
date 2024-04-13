connection_ap_warn_and_unmark_if_pending_circ(entry_connection_t *entry_conn,
                                              const char *where)
{
  if (pending_entry_connections &&
      smartlist_contains(pending_entry_connections, entry_conn)) {
    log_warn(LD_BUG, "What was %p doing in pending_entry_connections in %s?",
             entry_conn, where);
    connection_ap_mark_as_non_pending_circuit(entry_conn);
  }
}