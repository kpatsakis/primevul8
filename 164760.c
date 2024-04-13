connection_ap_mark_as_waiting_for_renddesc(entry_connection_t *entry_conn)
{
  tor_assert(entry_conn);

  connection_ap_mark_as_non_pending_circuit(entry_conn);
  ENTRY_TO_CONN(entry_conn)->state = AP_CONN_STATE_RENDDESC_WAIT;
}