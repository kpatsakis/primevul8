connection_half_edge_add(const edge_connection_t *conn,
                         origin_circuit_t *circ)
{
  half_edge_t *half_conn = NULL;
  int insert_at = 0;
  int ignored;

  /* Double-check for re-insertion. This should not happen,
   * but this check is cheap compared to the sort anyway */
  if (connection_half_edge_find_stream_id(circ->half_streams,
                                          conn->stream_id)) {
    log_warn(LD_BUG, "Duplicate stream close for stream %d on circuit %d",
             conn->stream_id, circ->global_identifier);
    return;
  }

  half_conn = tor_malloc_zero(sizeof(half_edge_t));
  ++n_half_conns_allocated;

  if (!circ->half_streams) {
    circ->half_streams = smartlist_new();
  }

  half_conn->stream_id = conn->stream_id;

  // How many sendme's should I expect?
  half_conn->sendmes_pending =
   (STREAMWINDOW_START-conn->package_window)/STREAMWINDOW_INCREMENT;

   // Is there a connected cell pending?
  half_conn->connected_pending = conn->base_.state ==
      AP_CONN_STATE_CONNECT_WAIT;

  /* Data should only arrive if we're not waiting on a resolved cell.
   * It can arrive after waiting on connected, because of optimistic
   * data. */
  if (conn->base_.state != AP_CONN_STATE_RESOLVE_WAIT) {
    // How many more data cells can arrive on this id?
    half_conn->data_pending = conn->deliver_window;
  }

  insert_at = smartlist_bsearch_idx(circ->half_streams, &half_conn->stream_id,
                                    connection_half_edge_compare_bsearch,
                                    &ignored);
  smartlist_insert(circ->half_streams, insert_at, half_conn);
}