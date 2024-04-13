connected_cell_format_payload(uint8_t *payload_out,
                              const tor_addr_t *addr,
                              uint32_t ttl)
{
  const sa_family_t family = tor_addr_family(addr);
  int connected_payload_len;

  /* should be needless */
  memset(payload_out, 0, MAX_CONNECTED_CELL_PAYLOAD_LEN);

  if (family == AF_INET) {
    set_uint32(payload_out, tor_addr_to_ipv4n(addr));
    connected_payload_len = 4;
  } else if (family == AF_INET6) {
    set_uint32(payload_out, 0);
    set_uint8(payload_out + 4, 6);
    memcpy(payload_out + 5, tor_addr_to_in6_addr8(addr), 16);
    connected_payload_len = 21;
  } else {
    return -1;
  }

  set_uint32(payload_out + connected_payload_len, htonl(clip_dns_ttl(ttl)));
  connected_payload_len += 4;

  tor_assert(connected_payload_len <= MAX_CONNECTED_CELL_PAYLOAD_LEN);

  return connected_payload_len;
}