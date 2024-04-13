tell_controller_about_resolved_result(entry_connection_t *conn,
                                      int answer_type,
                                      size_t answer_len,
                                      const char *answer,
                                      int ttl,
                                      time_t expires)
{
  expires = time(NULL) + ttl;
  if (answer_type == RESOLVED_TYPE_IPV4 && answer_len >= 4) {
    char *cp = tor_dup_ip(ntohl(get_uint32(answer)));
    if (cp)
      control_event_address_mapped(conn->socks_request->address,
                                   cp, expires, NULL, 0);
    tor_free(cp);
  } else if (answer_type == RESOLVED_TYPE_HOSTNAME && answer_len < 256) {
    char *cp = tor_strndup(answer, answer_len);
    control_event_address_mapped(conn->socks_request->address,
                                 cp, expires, NULL, 0);
    tor_free(cp);
  } else {
    control_event_address_mapped(conn->socks_request->address,
                                 "<error>", time(NULL)+ttl,
                                 "error=yes", 0);
  }
}