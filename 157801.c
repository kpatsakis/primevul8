fetch_var_cell_from_evbuffer(struct evbuffer *buf, var_cell_t **out,
                             int linkproto)
{
  char *hdr = NULL;
  int free_hdr = 0;
  size_t n;
  size_t buf_len;
  uint8_t command;
  uint16_t cell_length;
  var_cell_t *cell;
  int result = 0;
  const int wide_circ_ids = linkproto >= MIN_LINK_PROTO_FOR_WIDE_CIRC_IDS;
  const int circ_id_len = get_circ_id_size(wide_circ_ids);
  const unsigned header_len = get_var_cell_header_size(wide_circ_ids);

  *out = NULL;
  buf_len = evbuffer_get_length(buf);
  if (buf_len < header_len)
    return 0;

  n = inspect_evbuffer(buf, &hdr, header_len, &free_hdr, NULL);
  tor_assert(n >= header_len);

  command = get_uint8(hdr + circ_id_len);
  if (!(cell_command_is_var_length(command, linkproto))) {
    goto done;
  }

  cell_length = ntohs(get_uint16(hdr + circ_id_len + 1));
  if (buf_len < (size_t)(header_len+cell_length)) {
    result = 1; /* Not all here yet. */
    goto done;
  }

  cell = var_cell_new(cell_length);
  cell->command = command;
  if (wide_circ_ids)
    cell->circ_id = ntohl(get_uint32(hdr));
  else
    cell->circ_id = ntohs(get_uint16(hdr));
  evbuffer_drain(buf, header_len);
  evbuffer_remove(buf, cell->payload, cell_length);
  *out = cell;
  result = 1;

 done:
  if (free_hdr && hdr)
    tor_free(hdr);
  return result;
}