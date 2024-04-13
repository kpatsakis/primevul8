connection_edge_flushed_some(edge_connection_t *conn)
{
  switch (conn->base_.state) {
    case AP_CONN_STATE_OPEN:
      if (! conn->base_.linked) {
        note_user_activity(approx_time());
      }

      FALLTHROUGH;
    case EXIT_CONN_STATE_OPEN:
      sendme_connection_edge_consider_sending(conn);
      break;
  }
  return 0;
}