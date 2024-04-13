flush_chunk_tls(tor_tls_t *tls, buf_t *buf, chunk_t *chunk,
                size_t sz, size_t *buf_flushlen)
{
  int r;
  size_t forced;
  char *data;

  forced = tor_tls_get_forced_write_size(tls);
  if (forced > sz)
    sz = forced;
  if (chunk) {
    data = chunk->data;
    tor_assert(sz <= chunk->datalen);
  } else {
    data = NULL;
    tor_assert(sz == 0);
  }
  r = tor_tls_write(tls, data, sz);
  if (r < 0)
    return r;
  if (*buf_flushlen > (size_t)r)
    *buf_flushlen -= r;
  else
    *buf_flushlen = 0;
  buf_remove_from_front(buf, r);
  log_debug(LD_NET,"flushed %d bytes, %d ready to flush, %d remain.",
            r,(int)*buf_flushlen,(int)buf->datalen);
  return r;
}