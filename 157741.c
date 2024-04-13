write_to_evbuffer_zlib(struct evbuffer *buf, tor_zlib_state_t *state,
                       const char *data, size_t data_len,
                       int done)
{
  char *next;
  size_t old_avail, avail;
  int over = 0, n;
  struct evbuffer_iovec vec[1];
  do {
    {
      size_t cap = data_len / 4;
      if (cap < 128)
        cap = 128;
      /* XXXX NM this strategy is fragmentation-prone. We should really have
       * two iovecs, and write first into the one, and then into the
       * second if the first gets full. */
      n = evbuffer_reserve_space(buf, cap, vec, 1);
      tor_assert(n == 1);
    }

    next = vec[0].iov_base;
    avail = old_avail = vec[0].iov_len;

    switch (tor_zlib_process(state, &next, &avail, &data, &data_len, done)) {
      case TOR_ZLIB_DONE:
        over = 1;
        break;
      case TOR_ZLIB_ERR:
        return -1;
      case TOR_ZLIB_OK:
        if (data_len == 0)
          over = 1;
        break;
      case TOR_ZLIB_BUF_FULL:
        if (avail) {
          /* Zlib says we need more room (ZLIB_BUF_FULL).  Start a new chunk
           * automatically, whether were going to or not. */
        }
        break;
    }

    /* XXXX possible infinite loop on BUF_FULL. */
    vec[0].iov_len = old_avail - avail;
    evbuffer_commit_space(buf, vec, 1);

  } while (!over);
  check();
  return 0;
}