connection_ap_process_http_connect(entry_connection_t *conn)
{
  if (BUG(ENTRY_TO_CONN(conn)->state != AP_CONN_STATE_HTTP_CONNECT_WAIT))
    return -1;

  char *headers = NULL, *body = NULL;
  char *command = NULL, *addrport = NULL;
  char *addr = NULL;
  size_t bodylen = 0;

  const char *errmsg = NULL;
  int rv = 0;

  const int http_status =
    fetch_from_buf_http(ENTRY_TO_CONN(conn)->inbuf, &headers, 8192,
                        &body, &bodylen, 1024, 0);
  if (http_status < 0) {
    /* Bad http status */
    errmsg = "HTTP/1.0 400 Bad Request\r\n\r\n";
    goto err;
  } else if (http_status == 0) {
    /* no HTTP request yet. */
    goto done;
  }

  const int cmd_status = parse_http_command(headers, &command, &addrport);
  if (cmd_status < 0) {
    errmsg = "HTTP/1.0 400 Bad Request\r\n\r\n";
    goto err;
  }
  tor_assert(command);
  tor_assert(addrport);
  if (strcasecmp(command, "connect")) {
    errmsg = HTTP_CONNECT_IS_NOT_AN_HTTP_PROXY_MSG;
    goto err;
  }

  tor_assert(conn->socks_request);
  socks_request_t *socks = conn->socks_request;
  uint16_t port;
  if (tor_addr_port_split(LOG_WARN, addrport, &addr, &port) < 0) {
    errmsg = "HTTP/1.0 400 Bad Request\r\n\r\n";
    goto err;
  }
  if (strlen(addr) >= MAX_SOCKS_ADDR_LEN) {
    errmsg = "HTTP/1.0 414 Request-URI Too Long\r\n\r\n";
    goto err;
  }

  /* Abuse the 'username' and 'password' fields here. They are already an
  * abuse. */
  {
    char *authorization = http_get_header(headers, "Proxy-Authorization: ");
    if (authorization) {
      socks->username = authorization; // steal reference
      socks->usernamelen = strlen(authorization);
    }
    char *isolation = http_get_header(headers, "X-Tor-Stream-Isolation: ");
    if (isolation) {
      socks->password = isolation; // steal reference
      socks->passwordlen = strlen(isolation);
    }
  }

  socks->command = SOCKS_COMMAND_CONNECT;
  socks->listener_type = CONN_TYPE_AP_HTTP_CONNECT_LISTENER;
  strlcpy(socks->address, addr, sizeof(socks->address));
  socks->port = port;

  control_event_stream_status(conn, STREAM_EVENT_NEW, 0);

  rv = connection_ap_rewrite_and_attach_if_allowed(conn, NULL, NULL);

  // XXXX send a "100 Continue" message?

  goto done;

 err:
  if (BUG(errmsg == NULL))
    errmsg = "HTTP/1.0 400 Bad Request\r\n\r\n";
  log_info(LD_EDGE, "HTTP tunnel error: saying %s", escaped(errmsg));
  connection_buf_add(errmsg, strlen(errmsg), ENTRY_TO_CONN(conn));
  /* Mark it as "has_finished" so that we don't try to send an extra socks
   * reply. */
  conn->socks_request->has_finished = 1;
  connection_mark_unattached_ap(conn,
                                END_STREAM_REASON_HTTPPROTOCOL|
                                END_STREAM_REASON_FLAG_ALREADY_SOCKS_REPLIED);

 done:
  tor_free(headers);
  tor_free(body);
  tor_free(command);
  tor_free(addrport);
  tor_free(addr);
  return rv;
}