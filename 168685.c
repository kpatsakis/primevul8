connection_ap_handle_onion(entry_connection_t *conn,
                           socks_request_t *socks,
                           origin_circuit_t *circ)
{
  int retval;
  time_t now = approx_time();
  connection_t *base_conn = ENTRY_TO_CONN(conn);

  /* If .onion address requests are disabled, refuse the request */
  if (!conn->entry_cfg.onion_traffic) {
    log_warn(LD_APP, "Onion address %s requested from a port with .onion "
             "disabled", safe_str_client(socks->address));
    connection_mark_unattached_ap(conn, END_STREAM_REASON_ENTRYPOLICY);
    return -1;
  }

  /* Check whether it's RESOLVE or RESOLVE_PTR.  We don't handle those
   * for hidden service addresses. */
  if (SOCKS_COMMAND_IS_RESOLVE(socks->command)) {
    /* if it's a resolve request, fail it right now, rather than
     * building all the circuits and then realizing it won't work. */
    log_warn(LD_APP,
             "Resolve requests to hidden services not allowed. Failing.");
    connection_ap_handshake_socks_resolved(conn,RESOLVED_TYPE_ERROR,
                                           0,NULL,-1,TIME_MAX);
    connection_mark_unattached_ap(conn,
                               END_STREAM_REASON_SOCKSPROTOCOL |
                               END_STREAM_REASON_FLAG_ALREADY_SOCKS_REPLIED);
    return -1;
  }

  /* If we were passed a circuit, then we need to fail.  .onion addresses
   * only work when we launch our own circuits for now. */
  if (circ) {
    log_warn(LD_CONTROL, "Attachstream to a circuit is not "
             "supported for .onion addresses currently. Failing.");
    connection_mark_unattached_ap(conn, END_STREAM_REASON_TORPROTOCOL);
    return -1;
  }

  int descriptor_is_usable = 0;

  /* Create HS conn identifier with HS pubkey */
  hs_ident_edge_conn_t *hs_conn_ident =
    tor_malloc_zero(sizeof(hs_ident_edge_conn_t));

  retval = hs_parse_address(socks->address, &hs_conn_ident->identity_pk,
                            NULL, NULL);
  if (retval < 0) {
    log_warn(LD_GENERAL, "failed to parse hs address");
    tor_free(hs_conn_ident);
    return -1;
  }
  ENTRY_TO_EDGE_CONN(conn)->hs_ident = hs_conn_ident;

  /* Check the v3 desc cache */
  const hs_descriptor_t *cached_desc = NULL;
  unsigned int refetch_desc = 0;
  cached_desc = hs_cache_lookup_as_client(&hs_conn_ident->identity_pk);
  if (cached_desc) {
    descriptor_is_usable =
      hs_client_any_intro_points_usable(&hs_conn_ident->identity_pk,
                                        cached_desc);
    log_info(LD_GENERAL, "Found %s descriptor in cache for %s. %s.",
             (descriptor_is_usable) ? "usable" : "unusable",
             safe_str_client(socks->address),
             (descriptor_is_usable) ? "Not fetching." : "Refetching.");
  } else {
    /* We couldn't find this descriptor; we should look it up. */
    log_info(LD_REND, "No descriptor found in our cache for %s. Fetching.",
             safe_str_client(socks->address));
    refetch_desc = 1;
  }

  /* Help predict that we'll want to do hidden service circuits in the
   * future. We're not sure if it will need a stable circuit yet, but
   * we know we'll need *something*. */
  rep_hist_note_used_internal(now, 0, 1);

  /* Now we have a descriptor but is it usable or not? If not, refetch.
   * Also, a fetch could have been requested if the onion address was not
   * found in the cache previously. */
  if (refetch_desc || !descriptor_is_usable) {
    edge_connection_t *edge_conn = ENTRY_TO_EDGE_CONN(conn);
    connection_ap_mark_as_non_pending_circuit(conn);
    base_conn->state = AP_CONN_STATE_RENDDESC_WAIT;
    tor_assert(edge_conn->hs_ident);
    /* Attempt to fetch the hsv3 descriptor. Check the retval to see how it
     * went and act accordingly. */
    int ret = hs_client_refetch_hsdesc(&edge_conn->hs_ident->identity_pk);
    switch (ret) {
    case HS_CLIENT_FETCH_MISSING_INFO:
      /* Keeping the connection in descriptor wait state is fine because
       * once we get enough dirinfo or a new live consensus, the HS client
       * subsystem is notified and every connection in that state will
       * trigger a fetch for the service key. */
    case HS_CLIENT_FETCH_LAUNCHED:
    case HS_CLIENT_FETCH_PENDING:
    case HS_CLIENT_FETCH_HAVE_DESC:
      return 0;
    case HS_CLIENT_FETCH_ERROR:
    case HS_CLIENT_FETCH_NO_HSDIRS:
    case HS_CLIENT_FETCH_NOT_ALLOWED:
      /* Can't proceed further and better close the SOCKS request. */
      return -1;
    }
  }

  /* We have the descriptor!  So launch a connection to the HS. */
  log_info(LD_REND, "Descriptor is here. Great.");

  base_conn->state = AP_CONN_STATE_CIRCUIT_WAIT;
  /* We'll try to attach it at the next event loop, or whenever
   * we call connection_ap_attach_pending() */
  connection_ap_mark_as_pending_circuit(conn);
  return 0;
}