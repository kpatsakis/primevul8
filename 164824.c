connection_ap_process_natd(entry_connection_t *conn)
{
  char tmp_buf[36], *tbuf, *daddr;
  size_t tlen = 30;
  int err, port_ok;
  socks_request_t *socks;

  tor_assert(conn);
  tor_assert(ENTRY_TO_CONN(conn)->state == AP_CONN_STATE_NATD_WAIT);
  tor_assert(conn->socks_request);
  socks = conn->socks_request;

  log_debug(LD_APP,"entered.");

  /* look for LF-terminated "[DEST ip_addr port]"
   * where ip_addr is a dotted-quad and port is in string form */
  err = connection_buf_get_line(ENTRY_TO_CONN(conn), tmp_buf, &tlen);
  if (err == 0)
    return 0;
  if (err < 0) {
    log_warn(LD_APP,"NATD handshake failed (DEST too long). Closing");
    connection_mark_unattached_ap(conn, END_STREAM_REASON_INVALID_NATD_DEST);
    return -1;
  }

  if (strcmpstart(tmp_buf, "[DEST ")) {
    log_warn(LD_APP,"NATD handshake was ill-formed; closing. The client "
             "said: %s",
             escaped(tmp_buf));
    connection_mark_unattached_ap(conn, END_STREAM_REASON_INVALID_NATD_DEST);
    return -1;
  }

  daddr = tbuf = &tmp_buf[0] + 6; /* after end of "[DEST " */
  if (!(tbuf = strchr(tbuf, ' '))) {
    log_warn(LD_APP,"NATD handshake was ill-formed; closing. The client "
             "said: %s",
             escaped(tmp_buf));
    connection_mark_unattached_ap(conn, END_STREAM_REASON_INVALID_NATD_DEST);
    return -1;
  }
  *tbuf++ = '\0';

  /* pretend that a socks handshake completed so we don't try to
   * send a socks reply down a natd conn */
  strlcpy(socks->address, daddr, sizeof(socks->address));
  socks->port = (uint16_t)
    tor_parse_long(tbuf, 10, 1, 65535, &port_ok, &daddr);
  if (!port_ok) {
    log_warn(LD_APP,"NATD handshake failed; port %s is ill-formed or out "
             "of range.", escaped(tbuf));
    connection_mark_unattached_ap(conn, END_STREAM_REASON_INVALID_NATD_DEST);
    return -1;
  }

  socks->command = SOCKS_COMMAND_CONNECT;
  socks->has_finished = 1;

  control_event_stream_status(conn, STREAM_EVENT_NEW, 0);

  ENTRY_TO_CONN(conn)->state = AP_CONN_STATE_CIRCUIT_WAIT;

  return connection_ap_rewrite_and_attach_if_allowed(conn, NULL, NULL);
}