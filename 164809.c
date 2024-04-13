connection_ap_handshake_process_socks(entry_connection_t *conn)
{
  socks_request_t *socks;
  int sockshere;
  const or_options_t *options = get_options();
  int had_reply = 0;
  connection_t *base_conn = ENTRY_TO_CONN(conn);

  tor_assert(conn);
  tor_assert(base_conn->type == CONN_TYPE_AP);
  tor_assert(base_conn->state == AP_CONN_STATE_SOCKS_WAIT);
  tor_assert(conn->socks_request);
  socks = conn->socks_request;

  log_debug(LD_APP,"entered.");

  sockshere = fetch_from_buf_socks(base_conn->inbuf, socks,
                                   options->TestSocks, options->SafeSocks);

  if (socks->replylen) {
    had_reply = 1;
    connection_buf_add((const char*)socks->reply, socks->replylen,
                            base_conn);
    socks->replylen = 0;
    if (sockshere == -1) {
      /* An invalid request just got a reply, no additional
       * one is necessary. */
      socks->has_finished = 1;
    }
  }

  if (sockshere == 0) {
    log_debug(LD_APP,"socks handshake not all here yet.");
    return 0;
  } else if (sockshere == -1) {
    if (!had_reply) {
      log_warn(LD_APP,"Fetching socks handshake failed. Closing.");
      connection_ap_handshake_socks_reply(conn, NULL, 0,
                                          END_STREAM_REASON_SOCKSPROTOCOL);
    }
    connection_mark_unattached_ap(conn,
                              END_STREAM_REASON_SOCKSPROTOCOL |
                              END_STREAM_REASON_FLAG_ALREADY_SOCKS_REPLIED);
    return -1;
  } /* else socks handshake is done, continue processing */

  if (SOCKS_COMMAND_IS_CONNECT(socks->command))
    control_event_stream_status(conn, STREAM_EVENT_NEW, 0);
  else
    control_event_stream_status(conn, STREAM_EVENT_NEW_RESOLVE, 0);

  return connection_ap_rewrite_and_attach_if_allowed(conn, NULL, NULL);
}