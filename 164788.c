connection_ap_get_original_destination(entry_connection_t *conn,
                                       socks_request_t *req)
{
#ifdef TRANS_NETFILTER
  return destination_from_socket(conn, req);
#elif defined(TRANS_PF)
  const or_options_t *options = get_options();

  if (options->TransProxyType_parsed == TPT_PF_DIVERT)
    return destination_from_socket(conn, req);

  if (options->TransProxyType_parsed == TPT_DEFAULT ||
      options->TransProxyType_parsed == TPT_IPFW)
    return destination_from_pf(conn, req);

  (void)conn;
  (void)req;
  log_warn(LD_BUG, "Proxy destination determination mechanism %s unknown.",
           options->TransProxyType);
  return -1;
#else
  (void)conn;
  (void)req;
  log_warn(LD_BUG, "Called connection_ap_get_original_destination, but no "
           "transparent proxy method was configured.");
  return -1;
#endif /* defined(TRANS_NETFILTER) || ... */
}