read_to_chunk(buf_t *buf, chunk_t *chunk, tor_socket_t fd, size_t at_most,
              int *reached_eof, int *socket_error)
{
  ssize_t read_result;
  if (at_most > CHUNK_REMAINING_CAPACITY(chunk))
    at_most = CHUNK_REMAINING_CAPACITY(chunk);
  read_result = tor_socket_recv(fd, CHUNK_WRITE_PTR(chunk), at_most, 0);

  if (read_result < 0) {
    int e = tor_socket_errno(fd);
    if (!ERRNO_IS_EAGAIN(e)) { /* it's a real error */
#ifdef _WIN32
      if (e == WSAENOBUFS)
        log_warn(LD_NET,"recv() failed: WSAENOBUFS. Not enough ram?");
#endif
      *socket_error = e;
      return -1;
    }
    return 0; /* would block. */
  } else if (read_result == 0) {
    log_debug(LD_NET,"Encountered eof on fd %d", (int)fd);
    *reached_eof = 1;
    return 0;
  } else { /* actually got bytes. */
    buf->datalen += read_result;
    chunk->datalen += read_result;
    log_debug(LD_NET,"Read %ld bytes. %d on inbuf.", (long)read_result,
              (int)buf->datalen);
    tor_assert(read_result < INT_MAX);
    return (int)read_result;
  }
}