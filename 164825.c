connection_ap_handshake_rewrite(entry_connection_t *conn,
                                rewrite_result_t *out)
{
  socks_request_t *socks = conn->socks_request;
  const or_options_t *options = get_options();
  tor_addr_t addr_tmp;

  /* Initialize all the fields of 'out' to reasonable defaults */
  out->automap = 0;
  out->exit_source = ADDRMAPSRC_NONE;
  out->map_expires = TIME_MAX;
  out->end_reason = 0;
  out->should_close = 0;
  out->orig_address[0] = 0;

  /* We convert all incoming addresses to lowercase. */
  tor_strlower(socks->address);
  /* Remember the original address. */
  strlcpy(out->orig_address, socks->address, sizeof(out->orig_address));
  log_debug(LD_APP,"Client asked for %s:%d",
            safe_str_client(socks->address),
            socks->port);

  /* Check for whether this is a .exit address.  By default, those are
   * disallowed when they're coming straight from the client, but you're
   * allowed to have them in MapAddress commands and so forth. */
  if (!strcmpend(socks->address, ".exit")) {
    static ratelim_t exit_warning_limit = RATELIM_INIT(60*15);
    log_fn_ratelim(&exit_warning_limit, LOG_WARN, LD_APP,
                   "The  \".exit\" notation is disabled in Tor due to "
                   "security risks.");
    control_event_client_status(LOG_WARN, "SOCKS_BAD_HOSTNAME HOSTNAME=%s",
                                escaped(socks->address));
    out->end_reason = END_STREAM_REASON_TORPROTOCOL;
    out->should_close = 1;
    return;
  }

  /* Remember the original address so we can tell the user about what
   * they actually said, not just what it turned into. */
  /* XXX yes, this is the same as out->orig_address above. One is
   * in the output, and one is in the connection. */
  if (! conn->original_dest_address) {
    /* Is the 'if' necessary here? XXXX */
    conn->original_dest_address = tor_strdup(conn->socks_request->address);
  }

  /* First, apply MapAddress and MAPADDRESS mappings. We need to do
   * these only for non-reverse lookups, since they don't exist for those.
   * We also need to do this before we consider automapping, since we might
   * e.g. resolve irc.oftc.net into irconionaddress.onion, at which point
   * we'd need to automap it. */
  if (socks->command != SOCKS_COMMAND_RESOLVE_PTR) {
    const unsigned rewrite_flags = AMR_FLAG_USE_MAPADDRESS;
    if (addressmap_rewrite(socks->address, sizeof(socks->address),
                       rewrite_flags, &out->map_expires, &out->exit_source)) {
      control_event_stream_status(conn, STREAM_EVENT_REMAP,
                                  REMAP_STREAM_SOURCE_CACHE);
    }
  }

  /* Now see if we need to create or return an existing Hostname->IP
   * automapping.  Automapping happens when we're asked to resolve a
   * hostname, and AutomapHostsOnResolve is set, and the hostname has a
   * suffix listed in AutomapHostsSuffixes.  It's a handy feature
   * that lets you have Tor assign e.g. IPv6 addresses for .onion
   * names, and return them safely from DNSPort.
   */
  if (socks->command == SOCKS_COMMAND_RESOLVE &&
      tor_addr_parse(&addr_tmp, socks->address)<0 &&
      options->AutomapHostsOnResolve) {
    /* Check the suffix... */
    out->automap = addressmap_address_should_automap(socks->address, options);
    if (out->automap) {
      /* If we get here, then we should apply an automapping for this. */
      const char *new_addr;
      /* We return an IPv4 address by default, or an IPv6 address if we
       * are allowed to do so. */
      int addr_type = RESOLVED_TYPE_IPV4;
      if (conn->socks_request->socks_version != 4) {
        if (!conn->entry_cfg.ipv4_traffic ||
            (conn->entry_cfg.ipv6_traffic && conn->entry_cfg.prefer_ipv6) ||
            conn->entry_cfg.prefer_ipv6_virtaddr)
          addr_type = RESOLVED_TYPE_IPV6;
      }
      /* Okay, register the target address as automapped, and find the new
       * address we're supposed to give as a resolve answer.  (Return a cached
       * value if we've looked up this address before.
       */
      new_addr = addressmap_register_virtual_address(
                                    addr_type, tor_strdup(socks->address));
      if (! new_addr) {
        log_warn(LD_APP, "Unable to automap address %s",
                 escaped_safe_str(socks->address));
        out->end_reason = END_STREAM_REASON_INTERNAL;
        out->should_close = 1;
        return;
      }
      log_info(LD_APP, "Automapping %s to %s",
               escaped_safe_str_client(socks->address),
               safe_str_client(new_addr));
      strlcpy(socks->address, new_addr, sizeof(socks->address));
    }
  }

  /* Now handle reverse lookups, if they're in the cache.  This doesn't
   * happen too often, since client-side DNS caching is off by default,
   * and very deprecated. */
  if (socks->command == SOCKS_COMMAND_RESOLVE_PTR) {
    unsigned rewrite_flags = 0;
    if (conn->entry_cfg.use_cached_ipv4_answers)
      rewrite_flags |= AMR_FLAG_USE_IPV4_DNS;
    if (conn->entry_cfg.use_cached_ipv6_answers)
      rewrite_flags |= AMR_FLAG_USE_IPV6_DNS;

    if (addressmap_rewrite_reverse(socks->address, sizeof(socks->address),
                                   rewrite_flags, &out->map_expires)) {
      char *result = tor_strdup(socks->address);
      /* remember _what_ is supposed to have been resolved. */
      tor_snprintf(socks->address, sizeof(socks->address), "REVERSE[%s]",
                   out->orig_address);
      connection_ap_handshake_socks_resolved(conn, RESOLVED_TYPE_HOSTNAME,
                                             strlen(result), (uint8_t*)result,
                                             -1,
                                             out->map_expires);
      tor_free(result);
      out->end_reason = END_STREAM_REASON_DONE |
                        END_STREAM_REASON_FLAG_ALREADY_SOCKS_REPLIED;
      out->should_close = 1;
      return;
    }

    /* Hang on, did we find an answer saying that this is a reverse lookup for
     * an internal address?  If so, we should reject it if we're configured to
     * do so. */
    if (options->ClientDNSRejectInternalAddresses) {
      /* Don't let clients try to do a reverse lookup on 10.0.0.1. */
      tor_addr_t addr;
      int ok;
      ok = tor_addr_parse_PTR_name(
                               &addr, socks->address, AF_UNSPEC, 1);
      if (ok == 1 && tor_addr_is_internal(&addr, 0)) {
        connection_ap_handshake_socks_resolved(conn, RESOLVED_TYPE_ERROR,
                                               0, NULL, -1, TIME_MAX);
        out->end_reason = END_STREAM_REASON_SOCKSPROTOCOL |
                          END_STREAM_REASON_FLAG_ALREADY_SOCKS_REPLIED;
        out->should_close = 1;
        return;
      }
    }
  }

  /* If we didn't automap it before, then this is still the address that
   * came straight from the user, mapped according to any
   * MapAddress/MAPADDRESS commands.  Now apply other mappings,
   * including previously registered Automap entries (IP back to
   * hostname), TrackHostExits entries, and client-side DNS cache
   * entries (if they're turned on).
   */
  if (socks->command != SOCKS_COMMAND_RESOLVE_PTR &&
      !out->automap) {
    unsigned rewrite_flags = AMR_FLAG_USE_AUTOMAP | AMR_FLAG_USE_TRACKEXIT;
    addressmap_entry_source_t exit_source2;
    if (conn->entry_cfg.use_cached_ipv4_answers)
      rewrite_flags |= AMR_FLAG_USE_IPV4_DNS;
    if (conn->entry_cfg.use_cached_ipv6_answers)
      rewrite_flags |= AMR_FLAG_USE_IPV6_DNS;
    if (addressmap_rewrite(socks->address, sizeof(socks->address),
                        rewrite_flags, &out->map_expires, &exit_source2)) {
      control_event_stream_status(conn, STREAM_EVENT_REMAP,
                                  REMAP_STREAM_SOURCE_CACHE);
    }
    if (out->exit_source == ADDRMAPSRC_NONE) {
      /* If it wasn't a .exit before, maybe it turned into a .exit. Remember
       * the original source of a .exit. */
      out->exit_source = exit_source2;
    }
  }

  /* Check to see whether we're about to use an address in the virtual
   * range without actually having gotten it from an Automap. */
  if (!out->automap && address_is_in_virtual_range(socks->address)) {
    /* This address was probably handed out by
     * client_dns_get_unmapped_address, but the mapping was discarded for some
     * reason.  Or the user typed in a virtual address range manually.  We
     * *don't* want to send the address through Tor; that's likely to fail,
     * and may leak information.
     */
    log_warn(LD_APP,"Missing mapping for virtual address '%s'. Refusing.",
             safe_str_client(socks->address));
    out->end_reason = END_STREAM_REASON_INTERNAL;
    out->should_close = 1;
    return;
  }
}