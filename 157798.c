fetch_from_buf_socks(buf_t *buf, socks_request_t *req,
                     int log_sockstype, int safe_socks)
{
  int res;
  ssize_t n_drain;
  size_t want_length = 128;

  if (buf->datalen < 2) /* version and another byte */
    return 0;

  do {
    n_drain = 0;
    buf_pullup(buf, want_length, 0);
    tor_assert(buf->head && buf->head->datalen >= 2);
    want_length = 0;

    res = parse_socks(buf->head->data, buf->head->datalen, req, log_sockstype,
                      safe_socks, &n_drain, &want_length);

    if (n_drain < 0)
      buf_clear(buf);
    else if (n_drain > 0)
      buf_remove_from_front(buf, n_drain);

  } while (res == 0 && buf->head && want_length < buf->datalen &&
           buf->datalen >= 2);

  return res;
}