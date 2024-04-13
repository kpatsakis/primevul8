connection_ap_get_begincell_flags(entry_connection_t *ap_conn)
{
  edge_connection_t *edge_conn = ENTRY_TO_EDGE_CONN(ap_conn);
  const node_t *exitnode = NULL;
  const crypt_path_t *cpath_layer = edge_conn->cpath_layer;
  uint32_t flags = 0;

  /* No flags for begindir */
  if (ap_conn->use_begindir)
    return 0;

  /* No flags for hidden services. */
  if (edge_conn->on_circuit->purpose != CIRCUIT_PURPOSE_C_GENERAL)
    return 0;

  /* If only IPv4 is supported, no flags */
  if (ap_conn->entry_cfg.ipv4_traffic && !ap_conn->entry_cfg.ipv6_traffic)
    return 0;

  if (! cpath_layer ||
      ! cpath_layer->extend_info)
    return 0;

  if (!ap_conn->entry_cfg.ipv4_traffic)
    flags |= BEGIN_FLAG_IPV4_NOT_OK;

  exitnode = node_get_by_id(cpath_layer->extend_info->identity_digest);

  if (ap_conn->entry_cfg.ipv6_traffic && exitnode) {
    tor_addr_t a;
    tor_addr_make_null(&a, AF_INET6);
    if (compare_tor_addr_to_node_policy(&a, ap_conn->socks_request->port,
                                        exitnode)
        != ADDR_POLICY_REJECTED) {
      /* Only say "IPv6 OK" if the exit node supports IPv6. Otherwise there's
       * no point. */
      flags |= BEGIN_FLAG_IPV6_OK;
    }
  }

  if (flags == BEGIN_FLAG_IPV6_OK) {
    /* When IPv4 and IPv6 are both allowed, consider whether to say we
     * prefer IPv6.  Otherwise there's no point in declaring a preference */
    if (ap_conn->entry_cfg.prefer_ipv6)
      flags |= BEGIN_FLAG_IPV6_PREFERRED;
  }

  if (flags == BEGIN_FLAG_IPV4_NOT_OK) {
    log_warn(LD_EDGE, "I'm about to ask a node for a connection that I "
             "am telling it to fulfil with neither IPv4 nor IPv6. That's "
             "not going to work. Did you perhaps ask for an IPv6 address "
             "on an IPv4Only port, or vice versa?");
  }

  return flags;
}