connection_ap_fail_onehop(const char *failed_digest,
                          cpath_build_state_t *build_state)
{
  entry_connection_t *entry_conn;
  char digest[DIGEST_LEN];
  smartlist_t *conns = get_connection_array();
  SMARTLIST_FOREACH_BEGIN(conns, connection_t *, conn) {
    if (conn->marked_for_close ||
        conn->type != CONN_TYPE_AP ||
        conn->state != AP_CONN_STATE_CIRCUIT_WAIT)
      continue;
    entry_conn = TO_ENTRY_CONN(conn);
    if (!entry_conn->want_onehop)
      continue;
    if (hexdigest_to_digest(entry_conn->chosen_exit_name, digest) < 0 ||
        tor_memneq(digest, failed_digest, DIGEST_LEN))
      continue;
    if (tor_digest_is_zero(digest)) {
      /* we don't know the digest; have to compare addr:port */
      tor_addr_t addr;
      if (!build_state || !build_state->chosen_exit ||
          !entry_conn->socks_request) {
        continue;
      }
      if (tor_addr_parse(&addr, entry_conn->socks_request->address)<0 ||
          !extend_info_has_orport(build_state->chosen_exit, &addr,
                                  entry_conn->socks_request->port))
        continue;
    }
    log_info(LD_APP, "Closing one-hop stream to '%s/%s' because the OR conn "
                     "just failed.", entry_conn->chosen_exit_name,
                     entry_conn->socks_request->address);
    connection_mark_unattached_ap(entry_conn, END_STREAM_REASON_TIMEOUT);
  } SMARTLIST_FOREACH_END(conn);
}