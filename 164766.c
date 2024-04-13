destination_from_socket(entry_connection_t *conn, socks_request_t *req)
{
  struct sockaddr_storage orig_dst;
  socklen_t orig_dst_len = sizeof(orig_dst);
  tor_addr_t addr;

#ifdef TRANS_TPROXY
  if (get_options()->TransProxyType_parsed == TPT_TPROXY) {
    if (getsockname(ENTRY_TO_CONN(conn)->s, (struct sockaddr*)&orig_dst,
                    &orig_dst_len) < 0) {
      int e = tor_socket_errno(ENTRY_TO_CONN(conn)->s);
      log_warn(LD_NET, "getsockname() failed: %s", tor_socket_strerror(e));
      return -1;
    }
    goto done;
  }
#endif /* defined(TRANS_TPROXY) */

#ifdef TRANS_NETFILTER
  int rv = -1;
  switch (ENTRY_TO_CONN(conn)->socket_family) {
#ifdef TRANS_NETFILTER_IPV4
    case AF_INET:
      rv = getsockopt(ENTRY_TO_CONN(conn)->s, SOL_IP, SO_ORIGINAL_DST,
                  (struct sockaddr*)&orig_dst, &orig_dst_len);
      break;
#endif /* defined(TRANS_NETFILTER_IPV4) */
#ifdef TRANS_NETFILTER_IPV6
    case AF_INET6:
      rv = getsockopt(ENTRY_TO_CONN(conn)->s, SOL_IPV6, IP6T_SO_ORIGINAL_DST,
                  (struct sockaddr*)&orig_dst, &orig_dst_len);
      break;
#endif /* defined(TRANS_NETFILTER_IPV6) */
    default:
      log_warn(LD_BUG, "Received transparent data from an unsupported "
                       "socket family %d",
               ENTRY_TO_CONN(conn)->socket_family);
      return -1;
  }
  if (rv < 0) {
    int e = tor_socket_errno(ENTRY_TO_CONN(conn)->s);
    log_warn(LD_NET, "getsockopt() failed: %s", tor_socket_strerror(e));
    return -1;
  }
  goto done;
#elif defined(TRANS_PF)
  if (getsockname(ENTRY_TO_CONN(conn)->s, (struct sockaddr*)&orig_dst,
                  &orig_dst_len) < 0) {
    int e = tor_socket_errno(ENTRY_TO_CONN(conn)->s);
    log_warn(LD_NET, "getsockname() failed: %s", tor_socket_strerror(e));
    return -1;
  }
  goto done;
#else
  (void)conn;
  (void)req;
  log_warn(LD_BUG, "Unable to determine destination from socket.");
  return -1;
#endif /* defined(TRANS_NETFILTER) || ... */

 done:
  tor_addr_from_sockaddr(&addr, (struct sockaddr*)&orig_dst, &req->port);
  tor_addr_to_str(req->address, &addr, sizeof(req->address), 1);

  return 0;
}