connection_ap_handshake_send_resolve(entry_connection_t *ap_conn)
{
  int payload_len, command;
  const char *string_addr;
  char inaddr_buf[REVERSE_LOOKUP_NAME_BUF_LEN];
  origin_circuit_t *circ;
  edge_connection_t *edge_conn = ENTRY_TO_EDGE_CONN(ap_conn);
  connection_t *base_conn = TO_CONN(edge_conn);
  tor_assert(edge_conn->on_circuit);
  circ = TO_ORIGIN_CIRCUIT(edge_conn->on_circuit);

  tor_assert(base_conn->type == CONN_TYPE_AP);
  tor_assert(base_conn->state == AP_CONN_STATE_CIRCUIT_WAIT);
  tor_assert(ap_conn->socks_request);
  tor_assert(circ->base_.purpose == CIRCUIT_PURPOSE_C_GENERAL);

  command = ap_conn->socks_request->command;
  tor_assert(SOCKS_COMMAND_IS_RESOLVE(command));

  edge_conn->stream_id = get_unique_stream_id_by_circ(circ);
  if (edge_conn->stream_id==0) {
    /* XXXX+ Instead of closing this stream, we should make it get
     * retried on another circuit. */
    connection_mark_unattached_ap(ap_conn, END_STREAM_REASON_INTERNAL);

    /* Mark this circuit "unusable for new streams". */
    mark_circuit_unusable_for_new_conns(circ);
    return -1;
  }

  if (command == SOCKS_COMMAND_RESOLVE) {
    string_addr = ap_conn->socks_request->address;
    payload_len = (int)strlen(string_addr)+1;
  } else {
    /* command == SOCKS_COMMAND_RESOLVE_PTR */
    const char *a = ap_conn->socks_request->address;
    tor_addr_t addr;
    int r;

    /* We're doing a reverse lookup.  The input could be an IP address, or
     * could be an .in-addr.arpa or .ip6.arpa address */
    r = tor_addr_parse_PTR_name(&addr, a, AF_UNSPEC, 1);
    if (r <= 0) {
      log_warn(LD_APP, "Rejecting ill-formed reverse lookup of %s",
               safe_str_client(a));
      connection_mark_unattached_ap(ap_conn, END_STREAM_REASON_INTERNAL);
      return -1;
    }

    r = tor_addr_to_PTR_name(inaddr_buf, sizeof(inaddr_buf), &addr);
    if (r < 0) {
      log_warn(LD_BUG, "Couldn't generate reverse lookup hostname of %s",
               safe_str_client(a));
      connection_mark_unattached_ap(ap_conn, END_STREAM_REASON_INTERNAL);
      return -1;
    }

    string_addr = inaddr_buf;
    payload_len = (int)strlen(inaddr_buf)+1;
    tor_assert(payload_len <= (int)sizeof(inaddr_buf));
  }

  log_debug(LD_APP,
            "Sending relay cell to begin stream %d.", edge_conn->stream_id);

  if (connection_edge_send_command(edge_conn,
                           RELAY_COMMAND_RESOLVE,
                           string_addr, payload_len) < 0)
    return -1; /* circuit is closed, don't continue */

  if (!base_conn->address) {
    /* This might be unnecessary. XXXX */
    base_conn->address = tor_addr_to_str_dup(&base_conn->addr);
  }
  base_conn->state = AP_CONN_STATE_RESOLVE_WAIT;
  log_info(LD_APP,"Address sent for resolve, ap socket "TOR_SOCKET_T_FORMAT
           ", n_circ_id %u",
           base_conn->s, (unsigned)circ->base_.n_circ_id);
  control_event_stream_status(ap_conn, STREAM_EVENT_SENT_RESOLVE, 0);
  return 0;
}