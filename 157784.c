fetch_var_cell_from_buf(buf_t *buf, var_cell_t **out, int linkproto)
{
  char hdr[VAR_CELL_MAX_HEADER_SIZE];
  var_cell_t *result;
  uint8_t command;
  uint16_t length;
  const int wide_circ_ids = linkproto >= MIN_LINK_PROTO_FOR_WIDE_CIRC_IDS;
  const int circ_id_len = get_circ_id_size(wide_circ_ids);
  const unsigned header_len = get_var_cell_header_size(wide_circ_ids);
  check();
  *out = NULL;
  if (buf->datalen < header_len)
    return 0;
  peek_from_buf(hdr, header_len, buf);

  command = get_uint8(hdr + circ_id_len);
  if (!(cell_command_is_var_length(command, linkproto)))
    return 0;

  length = ntohs(get_uint16(hdr + circ_id_len + 1));
  if (buf->datalen < (size_t)(header_len+length))
    return 1;
  result = var_cell_new(length);
  result->command = command;
  if (wide_circ_ids)
    result->circ_id = ntohl(get_uint32(hdr));
  else
    result->circ_id = ntohs(get_uint16(hdr));

  buf_remove_from_front(buf, header_len);
  peek_from_buf((char*) result->payload, length, buf);
  buf_remove_from_front(buf, length);
  check();

  *out = result;
  return 1;
}