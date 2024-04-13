destination_from_pf(entry_connection_t *conn, socks_request_t *req)
{
  struct sockaddr_storage proxy_addr;
  socklen_t proxy_addr_len = sizeof(proxy_addr);
  struct sockaddr *proxy_sa = (struct sockaddr*) &proxy_addr;
  struct pfioc_natlook pnl;
  tor_addr_t addr;
  int pf = -1;

  if (getsockname(ENTRY_TO_CONN(conn)->s, (struct sockaddr*)&proxy_addr,
                  &proxy_addr_len) < 0) {
    int e = tor_socket_errno(ENTRY_TO_CONN(conn)->s);
    log_warn(LD_NET, "getsockname() to determine transocks destination "
             "failed: %s", tor_socket_strerror(e));
    return -1;
  }

#ifdef __FreeBSD__
  if (get_options()->TransProxyType_parsed == TPT_IPFW) {
    /* ipfw(8) is used and in this case getsockname returned the original
       destination */
    if (tor_addr_from_sockaddr(&addr, proxy_sa, &req->port) < 0) {
      tor_fragile_assert();
      return -1;
    }

    tor_addr_to_str(req->address, &addr, sizeof(req->address), 0);

    return 0;
  }
#endif /* defined(__FreeBSD__) */

  memset(&pnl, 0, sizeof(pnl));
  pnl.proto           = IPPROTO_TCP;
  pnl.direction       = PF_OUT;
  if (proxy_sa->sa_family == AF_INET) {
    struct sockaddr_in *sin = (struct sockaddr_in *)proxy_sa;
    pnl.af              = AF_INET;
    pnl.saddr.v4.s_addr = tor_addr_to_ipv4n(&ENTRY_TO_CONN(conn)->addr);
    pnl.sport           = htons(ENTRY_TO_CONN(conn)->port);
    pnl.daddr.v4.s_addr = sin->sin_addr.s_addr;
    pnl.dport           = sin->sin_port;
  } else if (proxy_sa->sa_family == AF_INET6) {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)proxy_sa;
    pnl.af = AF_INET6;
    const struct in6_addr *dest_in6 =
      tor_addr_to_in6(&ENTRY_TO_CONN(conn)->addr);
    if (BUG(!dest_in6))
      return -1;
    memcpy(&pnl.saddr.v6, dest_in6, sizeof(struct in6_addr));
    pnl.sport = htons(ENTRY_TO_CONN(conn)->port);
    memcpy(&pnl.daddr.v6, &sin6->sin6_addr, sizeof(struct in6_addr));
    pnl.dport = sin6->sin6_port;
  } else {
    log_warn(LD_NET, "getsockname() gave an unexpected address family (%d)",
             (int)proxy_sa->sa_family);
    return -1;
  }

  pf = get_pf_socket();
  if (pf<0)
    return -1;

  if (ioctl(pf, DIOCNATLOOK, &pnl) < 0) {
    log_warn(LD_NET, "ioctl(DIOCNATLOOK) failed: %s", strerror(errno));
    return -1;
  }

  if (pnl.af == AF_INET) {
    tor_addr_from_ipv4n(&addr, pnl.rdaddr.v4.s_addr);
  } else if (pnl.af == AF_INET6) {
    tor_addr_from_in6(&addr, &pnl.rdaddr.v6);
  } else {
    tor_fragile_assert();
    return -1;
  }

  tor_addr_to_str(req->address, &addr, sizeof(req->address), 1);
  req->port = ntohs(pnl.rdport);

  return 0;
}