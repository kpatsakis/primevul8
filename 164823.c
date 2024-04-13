connection_ap_supports_optimistic_data(const entry_connection_t *conn)
{
  const edge_connection_t *edge_conn = ENTRY_TO_EDGE_CONN(conn);
  /* We can only send optimistic data if we're connected to an open
     general circuit. */
  if (edge_conn->on_circuit == NULL ||
      edge_conn->on_circuit->state != CIRCUIT_STATE_OPEN ||
      (edge_conn->on_circuit->purpose != CIRCUIT_PURPOSE_C_GENERAL &&
       edge_conn->on_circuit->purpose != CIRCUIT_PURPOSE_C_HSDIR_GET &&
       edge_conn->on_circuit->purpose != CIRCUIT_PURPOSE_S_HSDIR_POST &&
       edge_conn->on_circuit->purpose != CIRCUIT_PURPOSE_C_REND_JOINED))
    return 0;

  return conn->may_use_optimistic_data;
}