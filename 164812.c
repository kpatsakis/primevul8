connection_edge_end_errno(edge_connection_t *conn)
{
  uint8_t reason;
  tor_assert(conn);
  reason = errno_to_stream_end_reason(tor_socket_errno(conn->base_.s));
  return connection_edge_end(conn, reason);
}