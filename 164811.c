connection_ap_make_link(connection_t *partner,
                        char *address, uint16_t port,
                        const char *digest,
                        int session_group, int isolation_flags,
                        int use_begindir, int want_onehop)
{
  entry_connection_t *conn;
  connection_t *base_conn;

  log_info(LD_APP,"Making internal %s tunnel to %s:%d ...",
           want_onehop ? "direct" : "anonymized",
           safe_str_client(address), port);

  conn = entry_connection_new(CONN_TYPE_AP, tor_addr_family(&partner->addr));
  base_conn = ENTRY_TO_CONN(conn);
  base_conn->linked = 1; /* so that we can add it safely below. */

  /* populate conn->socks_request */

  /* leave version at zero, so the socks_reply is empty */
  conn->socks_request->socks_version = 0;
  conn->socks_request->has_finished = 0; /* waiting for 'connected' */
  strlcpy(conn->socks_request->address, address,
          sizeof(conn->socks_request->address));
  conn->socks_request->port = port;
  conn->socks_request->command = SOCKS_COMMAND_CONNECT;
  conn->want_onehop = want_onehop;
  conn->use_begindir = use_begindir;
  if (use_begindir) {
    conn->chosen_exit_name = tor_malloc(HEX_DIGEST_LEN+2);
    conn->chosen_exit_name[0] = '$';
    tor_assert(digest);
    base16_encode(conn->chosen_exit_name+1,HEX_DIGEST_LEN+1,
                  digest, DIGEST_LEN);
  }

  /* Populate isolation fields. */
  conn->socks_request->listener_type = CONN_TYPE_DIR_LISTENER;
  conn->original_dest_address = tor_strdup(address);
  conn->entry_cfg.session_group = session_group;
  conn->entry_cfg.isolation_flags = isolation_flags;

  base_conn->address = tor_strdup("(Tor_internal)");
  tor_addr_make_unspec(&base_conn->addr);
  base_conn->port = 0;

  connection_link_connections(partner, base_conn);

  if (connection_add(base_conn) < 0) { /* no space, forget it */
    connection_free(base_conn);
    return NULL;
  }

  base_conn->state = AP_CONN_STATE_CIRCUIT_WAIT;

  control_event_stream_status(conn, STREAM_EVENT_NEW, 0);

  /* attaching to a dirty circuit is fine */
  connection_ap_mark_as_pending_circuit(conn);
  log_info(LD_APP,"... application connection created and linked.");
  return conn;
}