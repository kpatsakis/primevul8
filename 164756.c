begin_cell_parse(const cell_t *cell, begin_cell_t *bcell,
                 uint8_t *end_reason_out)
{
  relay_header_t rh;
  const uint8_t *body, *nul;

  memset(bcell, 0, sizeof(*bcell));
  *end_reason_out = END_STREAM_REASON_MISC;

  relay_header_unpack(&rh, cell->payload);
  if (rh.length > RELAY_PAYLOAD_SIZE) {
    return -2; /*XXXX why not TORPROTOCOL? */
  }

  bcell->stream_id = rh.stream_id;

  if (rh.command == RELAY_COMMAND_BEGIN_DIR) {
    bcell->is_begindir = 1;
    return 0;
  } else if (rh.command != RELAY_COMMAND_BEGIN) {
    log_warn(LD_BUG, "Got an unexpected command %d", (int)rh.command);
    *end_reason_out = END_STREAM_REASON_INTERNAL;
    return -1;
  }

  body = cell->payload + RELAY_HEADER_SIZE;
  nul = memchr(body, 0, rh.length);
  if (! nul) {
    log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
           "Relay begin cell has no \\0. Closing.");
    *end_reason_out = END_STREAM_REASON_TORPROTOCOL;
    return -1;
  }

  if (tor_addr_port_split(LOG_PROTOCOL_WARN,
                          (char*)(body),
                          &bcell->address,&bcell->port)<0) {
    log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
           "Unable to parse addr:port in relay begin cell. Closing.");
    *end_reason_out = END_STREAM_REASON_TORPROTOCOL;
    return -1;
  }
  if (bcell->port == 0) {
    log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
           "Missing port in relay begin cell. Closing.");
    tor_free(bcell->address);
    *end_reason_out = END_STREAM_REASON_TORPROTOCOL;
    return -1;
  }
  if (body + rh.length >= nul + 4)
    bcell->flags = ntohl(get_uint32(nul+1));

  return 0;
}