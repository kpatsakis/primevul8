connection_ap_about_to_close(entry_connection_t *entry_conn)
{
  circuit_t *circ;
  edge_connection_t *edge_conn = ENTRY_TO_EDGE_CONN(entry_conn);
  connection_t *conn = ENTRY_TO_CONN(entry_conn);

  connection_edge_about_to_close(edge_conn);

  if (entry_conn->socks_request->has_finished == 0) {
    /* since conn gets removed right after this function finishes,
     * there's no point trying to send back a reply at this point. */
    log_warn(LD_BUG,"Closing stream (marked at %s:%d) without sending"
             " back a socks reply.",
             conn->marked_for_close_file, conn->marked_for_close);
  }
  if (!edge_conn->end_reason) {
    log_warn(LD_BUG,"Closing stream (marked at %s:%d) without having"
             " set end_reason.",
             conn->marked_for_close_file, conn->marked_for_close);
  }
  if (entry_conn->dns_server_request) {
    log_warn(LD_BUG,"Closing stream (marked at %s:%d) without having"
             " replied to DNS request.",
             conn->marked_for_close_file, conn->marked_for_close);
    dnsserv_reject_request(entry_conn);
  }

  if (TO_CONN(edge_conn)->state == AP_CONN_STATE_CIRCUIT_WAIT) {
    smartlist_remove(pending_entry_connections, entry_conn);
  }

#if 1
  /* Check to make sure that this isn't in pending_entry_connections if it
   * didn't actually belong there. */
  if (TO_CONN(edge_conn)->type == CONN_TYPE_AP) {
    connection_ap_warn_and_unmark_if_pending_circ(entry_conn,
                                                  "about_to_close");
  }
#endif /* 1 */

  control_event_stream_bandwidth(edge_conn);
  control_event_stream_status(entry_conn, STREAM_EVENT_CLOSED,
                              edge_conn->end_reason);
  circ = circuit_get_by_edge_conn(edge_conn);
  if (circ)
    circuit_detach_stream(circ, edge_conn);
}