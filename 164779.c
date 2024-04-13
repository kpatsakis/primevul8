circuit_discard_optional_exit_enclaves(extend_info_t *info)
{
  entry_connection_t *entry_conn;
  const node_t *r1, *r2;

  smartlist_t *conns = get_connection_array();
  SMARTLIST_FOREACH_BEGIN(conns, connection_t *, conn) {
    if (conn->marked_for_close ||
        conn->type != CONN_TYPE_AP ||
        conn->state != AP_CONN_STATE_CIRCUIT_WAIT)
      continue;
    entry_conn = TO_ENTRY_CONN(conn);
    if (!entry_conn->chosen_exit_optional &&
        !entry_conn->chosen_exit_retries)
      continue;
    r1 = node_get_by_nickname(entry_conn->chosen_exit_name,
                              NNF_NO_WARN_UNNAMED);
    r2 = node_get_by_id(info->identity_digest);
    if (!r1 || !r2 || r1 != r2)
      continue;
    tor_assert(entry_conn->socks_request);
    if (entry_conn->chosen_exit_optional) {
      log_info(LD_APP, "Giving up on enclave exit '%s' for destination %s.",
               safe_str_client(entry_conn->chosen_exit_name),
               escaped_safe_str_client(entry_conn->socks_request->address));
      entry_conn->chosen_exit_optional = 0;
      tor_free(entry_conn->chosen_exit_name); /* clears it */
      /* if this port is dangerous, warn or reject it now that we don't
       * think it'll be using an enclave. */
      consider_plaintext_ports(entry_conn, entry_conn->socks_request->port);
    }
    if (entry_conn->chosen_exit_retries) {
      if (--entry_conn->chosen_exit_retries == 0) { /* give up! */
        clear_trackexithost_mappings(entry_conn->chosen_exit_name);
        tor_free(entry_conn->chosen_exit_name); /* clears it */
        /* if this port is dangerous, warn or reject it now that we don't
         * think it'll be using an enclave. */
        consider_plaintext_ports(entry_conn, entry_conn->socks_request->port);
      }
    }
  } SMARTLIST_FOREACH_END(conn);
}