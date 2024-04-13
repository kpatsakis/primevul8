inspect_evbuffer(struct evbuffer *buf, char **data_out, size_t n,
                 int *free_out, struct evbuffer_ptr *pos)
{
  int n_vecs, i;

  if (evbuffer_get_length(buf) < n)
    n = evbuffer_get_length(buf);
  if (n == 0)
    return 0;
  n_vecs = evbuffer_peek(buf, n, pos, NULL, 0);
  tor_assert(n_vecs > 0);
  if (n_vecs == 1) {
    struct evbuffer_iovec v;
    i = evbuffer_peek(buf, n, pos, &v, 1);
    tor_assert(i == 1);
    *data_out = v.iov_base;
    *free_out = 0;
    return v.iov_len;
  } else {
    ev_ssize_t copied;
    *data_out = tor_malloc(n);
    *free_out = 1;
    copied = evbuffer_copyout(buf, *data_out, n);
    tor_assert(copied >= 0 && (size_t)copied == n);
    return copied;
  }
}