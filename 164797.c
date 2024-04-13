connection_ap_process_transparent(entry_connection_t *conn)
{
  socks_request_t *socks;

  tor_assert(conn);
  tor_assert(conn->socks_request);
  socks = conn->socks_request;

  /* pretend that a socks handshake completed so we don't try to
   * send a socks reply down a transparent conn */
  socks->command = SOCKS_COMMAND_CONNECT;
  socks->has_finished = 1;

  log_debug(LD_APP,"entered.");

  if (connection_ap_get_original_destination(conn, socks) < 0) {
    log_warn(LD_APP,"Fetching original destination failed. Closing.");
    connection_mark_unattached_ap(conn,
                               END_STREAM_REASON_CANT_FETCH_ORIG_DEST);
    return -1;
  }
  /* we have the original destination */

  control_event_stream_status(conn, STREAM_EVENT_NEW, 0);

  return connection_ap_rewrite_and_attach_if_allowed(conn, NULL, NULL);
}