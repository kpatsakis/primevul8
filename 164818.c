connection_ap_handshake_socks_resolved_addr(entry_connection_t *conn,
                                            const tor_addr_t *answer,
                                            int ttl,
                                            time_t expires)
{
  if (tor_addr_family(answer) == AF_INET) {
    uint32_t a = tor_addr_to_ipv4n(answer); /* network order */
    connection_ap_handshake_socks_resolved(conn,RESOLVED_TYPE_IPV4,4,
                                           (uint8_t*)&a,
                                           ttl, expires);
  } else if (tor_addr_family(answer) == AF_INET6) {
    const uint8_t *a = tor_addr_to_in6_addr8(answer);
    connection_ap_handshake_socks_resolved(conn,RESOLVED_TYPE_IPV6,16,
                                           a,
                                           ttl, expires);
  } else {
    log_warn(LD_BUG, "Got called with address of unexpected family %d",
             tor_addr_family(answer));
    connection_ap_handshake_socks_resolved(conn,
                                           RESOLVED_TYPE_ERROR,0,NULL,-1,-1);
  }
}