fetch_ext_or_command_from_buf(buf_t *buf, ext_or_cmd_t **out)
{
  char hdr[EXT_OR_CMD_HEADER_SIZE];
  uint16_t len;

  check();
  if (buf->datalen < EXT_OR_CMD_HEADER_SIZE)
    return 0;
  peek_from_buf(hdr, sizeof(hdr), buf);
  len = ntohs(get_uint16(hdr+2));
  if (buf->datalen < (unsigned)len + EXT_OR_CMD_HEADER_SIZE)
    return 0;
  *out = ext_or_cmd_new(len);
  (*out)->cmd = ntohs(get_uint16(hdr));
  (*out)->len = len;
  buf_remove_from_front(buf, EXT_OR_CMD_HEADER_SIZE);
  fetch_from_buf((*out)->body, len, buf);
  return 1;
}