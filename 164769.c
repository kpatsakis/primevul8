connection_edge_finished_flushing(edge_connection_t *conn)
{
  tor_assert(conn);

  switch (conn->base_.state) {
    case AP_CONN_STATE_OPEN:
    case EXIT_CONN_STATE_OPEN:
      sendme_connection_edge_consider_sending(conn);
      return 0;
    case AP_CONN_STATE_SOCKS_WAIT:
    case AP_CONN_STATE_NATD_WAIT:
    case AP_CONN_STATE_RENDDESC_WAIT:
    case AP_CONN_STATE_CIRCUIT_WAIT:
    case AP_CONN_STATE_CONNECT_WAIT:
    case AP_CONN_STATE_CONTROLLER_WAIT:
    case AP_CONN_STATE_RESOLVE_WAIT:
    case AP_CONN_STATE_HTTP_CONNECT_WAIT:
      return 0;
    default:
      log_warn(LD_BUG, "Called in unexpected state %d.",conn->base_.state);
      tor_fragile_assert();
      return -1;
  }
  return 0;
}