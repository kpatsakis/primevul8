peek_evbuffer_has_control0_command(struct evbuffer *buf)
{
  int result = 0;
  if (evbuffer_get_length(buf) >= 4) {
    int free_out = 0;
    char *data = NULL;
    size_t n = inspect_evbuffer(buf, &data, 4, &free_out, NULL);
    uint16_t cmd;
    tor_assert(n >= 4);
    cmd = ntohs(get_uint16(data+2));
    if (cmd <= 0x14)
      result = 1;
    if (free_out)
      tor_free(data);
  }
  return result;
}