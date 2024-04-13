connection_edge_about_to_close(edge_connection_t *edge_conn)
{
  if (!edge_conn->edge_has_sent_end) {
    connection_t *conn = TO_CONN(edge_conn);
    log_warn(LD_BUG, "(Harmless.) Edge connection (marked at %s:%d) "
             "hasn't sent end yet?",
             conn->marked_for_close_file, conn->marked_for_close);
    tor_fragile_assert();
  }
}