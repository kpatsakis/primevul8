flush_chunk(tor_socket_t s, buf_t *buf, chunk_t *chunk, size_t sz,
            size_t *buf_flushlen)
{
  ssize_t write_result;

  if (sz > chunk->datalen)
    sz = chunk->datalen;
  write_result = tor_socket_send(s, chunk->data, sz, 0);

  if (write_result < 0) {
    int e = tor_socket_errno(s);
    if (!ERRNO_IS_EAGAIN(e)) { /* it's a real error */
#ifdef _WIN32
      if (e == WSAENOBUFS)
        log_warn(LD_NET,"write() failed: WSAENOBUFS. Not enough ram?");
#endif
      return -1;
    }
    log_debug(LD_NET,"write() would block, returning.");
    return 0;
  } else {
    *buf_flushlen -= write_result;
    buf_remove_from_front(buf, write_result);
    tor_assert(write_result < INT_MAX);
    return (int)write_result;
  }
}