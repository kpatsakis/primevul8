connection_ap_expire_beginning(void)
{
  edge_connection_t *conn;
  entry_connection_t *entry_conn;
  circuit_t *circ;
  time_t now = time(NULL);
  const or_options_t *options = get_options();
  int severity;
  int cutoff;
  int seconds_idle, seconds_since_born;
  smartlist_t *conns = get_connection_array();

  SMARTLIST_FOREACH_BEGIN(conns, connection_t *, base_conn) {
    if (base_conn->type != CONN_TYPE_AP || base_conn->marked_for_close)
      continue;
    entry_conn = TO_ENTRY_CONN(base_conn);
    conn = ENTRY_TO_EDGE_CONN(entry_conn);
    /* if it's an internal linked connection, don't yell its status. */
    severity = (tor_addr_is_null(&base_conn->addr) && !base_conn->port)
      ? LOG_INFO : LOG_NOTICE;
    seconds_idle = (int)( now - base_conn->timestamp_last_read_allowed );
    seconds_since_born = (int)( now - base_conn->timestamp_created );

    if (base_conn->state == AP_CONN_STATE_OPEN)
      continue;

    /* We already consider SocksTimeout in
     * connection_ap_handshake_attach_circuit(), but we need to consider
     * it here too because controllers that put streams in controller_wait
     * state never ask Tor to attach the circuit. */
    if (AP_CONN_STATE_IS_UNATTACHED(base_conn->state)) {
      if (seconds_since_born >= options->SocksTimeout) {
        log_fn(severity, LD_APP,
            "Tried for %d seconds to get a connection to %s:%d. "
            "Giving up. (%s)",
            seconds_since_born,
            safe_str_client(entry_conn->socks_request->address),
            entry_conn->socks_request->port,
            conn_state_to_string(CONN_TYPE_AP, base_conn->state));
        connection_mark_unattached_ap(entry_conn, END_STREAM_REASON_TIMEOUT);
      }
      continue;
    }

    /* We're in state connect_wait or resolve_wait now -- waiting for a
     * reply to our relay cell. See if we want to retry/give up. */

    cutoff = compute_retry_timeout(entry_conn);
    if (seconds_idle < cutoff)
      continue;
    circ = circuit_get_by_edge_conn(conn);
    if (!circ) { /* it's vanished? */
      log_info(LD_APP,"Conn is waiting (address %s), but lost its circ.",
               safe_str_client(entry_conn->socks_request->address));
      connection_mark_unattached_ap(entry_conn, END_STREAM_REASON_TIMEOUT);
      continue;
    }
    if (circ->purpose == CIRCUIT_PURPOSE_C_REND_JOINED) {
      if (seconds_idle >= options->SocksTimeout) {
        log_fn(severity, LD_REND,
               "Rend stream is %d seconds late. Giving up on address"
               " '%s.onion'.",
               seconds_idle,
               safe_str_client(entry_conn->socks_request->address));
        /* Roll back path bias use state so that we probe the circuit
         * if nothing else succeeds on it */
        pathbias_mark_use_rollback(TO_ORIGIN_CIRCUIT(circ));

        connection_edge_end(conn, END_STREAM_REASON_TIMEOUT);
        connection_mark_unattached_ap(entry_conn, END_STREAM_REASON_TIMEOUT);
      }
      continue;
    }

    if (circ->purpose != CIRCUIT_PURPOSE_C_GENERAL &&
        circ->purpose != CIRCUIT_PURPOSE_CONTROLLER &&
        circ->purpose != CIRCUIT_PURPOSE_C_HSDIR_GET &&
        circ->purpose != CIRCUIT_PURPOSE_S_HSDIR_POST &&
        circ->purpose != CIRCUIT_PURPOSE_C_MEASURE_TIMEOUT &&
        circ->purpose != CIRCUIT_PURPOSE_PATH_BIAS_TESTING) {
      log_warn(LD_BUG, "circuit->purpose == CIRCUIT_PURPOSE_C_GENERAL failed. "
               "The purpose on the circuit was %s; it was in state %s, "
               "path_state %s.",
               circuit_purpose_to_string(circ->purpose),
               circuit_state_to_string(circ->state),
               CIRCUIT_IS_ORIGIN(circ) ?
                pathbias_state_to_string(TO_ORIGIN_CIRCUIT(circ)->path_state) :
                "none");
    }
    log_fn(cutoff < 15 ? LOG_INFO : severity, LD_APP,
           "We tried for %d seconds to connect to '%s' using exit %s."
           " Retrying on a new circuit.",
           seconds_idle,
           safe_str_client(entry_conn->socks_request->address),
           conn->cpath_layer ?
             extend_info_describe(conn->cpath_layer->extend_info):
             "*unnamed*");
    /* send an end down the circuit */
    connection_edge_end(conn, END_STREAM_REASON_TIMEOUT);
    /* un-mark it as ending, since we're going to reuse it */
    conn->edge_has_sent_end = 0;
    conn->end_reason = 0;
    /* make us not try this circuit again, but allow
     * current streams on it to survive if they can */
    mark_circuit_unusable_for_new_conns(TO_ORIGIN_CIRCUIT(circ));

    /* give our stream another 'cutoff' seconds to try */
    conn->base_.timestamp_last_read_allowed += cutoff;
    if (entry_conn->num_socks_retries < 250) /* avoid overflow */
      entry_conn->num_socks_retries++;
    /* move it back into 'pending' state, and try to attach. */
    if (connection_ap_detach_retriable(entry_conn, TO_ORIGIN_CIRCUIT(circ),
                                       END_STREAM_REASON_TIMEOUT)<0) {
      if (!base_conn->marked_for_close)
        connection_mark_unattached_ap(entry_conn,
                                      END_STREAM_REASON_CANT_ATTACH);
    }
  } SMARTLIST_FOREACH_END(base_conn);
}