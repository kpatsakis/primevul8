connection_ap_mark_as_non_pending_circuit(entry_connection_t *entry_conn)
{
  if (PREDICT_UNLIKELY(NULL == pending_entry_connections))
    return;
  UNMARK();
  smartlist_remove(pending_entry_connections, entry_conn);
}