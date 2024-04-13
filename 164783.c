connection_exit_about_to_close(edge_connection_t *edge_conn)
{
  circuit_t *circ;
  connection_t *conn = TO_CONN(edge_conn);

  connection_edge_about_to_close(edge_conn);

  circ = circuit_get_by_edge_conn(edge_conn);
  if (circ)
    circuit_detach_stream(circ, edge_conn);
  if (conn->state == EXIT_CONN_STATE_RESOLVING) {
    connection_dns_remove(edge_conn);
  }
}