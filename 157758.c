fetch_from_buf_socks_client(buf_t *buf, int state, char **reason)
{
  ssize_t drain = 0;
  int r;
  if (buf->datalen < 2)
    return 0;

  buf_pullup(buf, MAX_SOCKS_MESSAGE_LEN, 0);
  tor_assert(buf->head && buf->head->datalen >= 2);

  r = parse_socks_client((uint8_t*)buf->head->data, buf->head->datalen,
                         state, reason, &drain);
  if (drain > 0)
    buf_remove_from_front(buf, drain);
  else if (drain < 0)
    buf_clear(buf);

  return r;
}