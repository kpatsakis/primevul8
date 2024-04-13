read_to_buf_tls(tor_tls_t *tls, size_t at_most, buf_t *buf)
{
  int r = 0;
  size_t total_read = 0;

  check_no_tls_errors();

  check();

  while (at_most > total_read) {
    size_t readlen = at_most - total_read;
    chunk_t *chunk;
    if (!buf->tail || CHUNK_REMAINING_CAPACITY(buf->tail) < MIN_READ_LEN) {
      chunk = buf_add_chunk_with_capacity(buf, at_most, 1);
      if (readlen > chunk->memlen)
        readlen = chunk->memlen;
    } else {
      size_t cap = CHUNK_REMAINING_CAPACITY(buf->tail);
      chunk = buf->tail;
      if (cap < readlen)
        readlen = cap;
    }

    r = read_to_chunk_tls(buf, chunk, tls, readlen);
    check();
    if (r < 0)
      return r; /* Error */
    tor_assert(total_read+r < INT_MAX);
    total_read += r;
    if ((size_t)r < readlen) /* eof, block, or no more to read. */
      break;
  }
  return (int)total_read;
}