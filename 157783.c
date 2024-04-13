fetch_from_evbuffer_socks_client(struct evbuffer *buf, int state,
                                 char **reason)
{
  ssize_t drain = 0;
  uint8_t *data;
  size_t datalen;
  int r;

  /* Linearize the SOCKS response in the buffer, up to 128 bytes.
   * (parse_socks_client shouldn't need to see anything beyond that.) */
  datalen = evbuffer_get_length(buf);
  if (datalen > MAX_SOCKS_MESSAGE_LEN)
    datalen = MAX_SOCKS_MESSAGE_LEN;
  data = evbuffer_pullup(buf, datalen);

  r = parse_socks_client(data, datalen, state, reason, &drain);
  if (drain > 0)
    evbuffer_drain(buf, drain);
  else if (drain < 0)
    evbuffer_drain(buf, evbuffer_get_length(buf));

  return r;
}