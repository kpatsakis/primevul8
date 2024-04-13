fetch_ext_or_command_from_evbuffer(struct evbuffer *buf, ext_or_cmd_t **out)
{
  char hdr[EXT_OR_CMD_HEADER_SIZE];
  uint16_t len;
  size_t buf_len = evbuffer_get_length(buf);

  if (buf_len < EXT_OR_CMD_HEADER_SIZE)
    return 0;
  evbuffer_copyout(buf, hdr, EXT_OR_CMD_HEADER_SIZE);
  len = ntohs(get_uint16(hdr+2));
  if (buf_len < (unsigned)len + EXT_OR_CMD_HEADER_SIZE)
    return 0;
  *out = ext_or_cmd_new(len);
  (*out)->cmd = ntohs(get_uint16(hdr));
  (*out)->len = len;
  evbuffer_drain(buf, EXT_OR_CMD_HEADER_SIZE);
  evbuffer_remove(buf, (*out)->body, len);
  return 1;
}