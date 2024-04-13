generic_buffer_set_to_copy(generic_buffer_t **output,
                           const generic_buffer_t *input)
{
#ifdef USE_BUFFEREVENTS
  struct evbuffer_ptr ptr;
  size_t remaining = evbuffer_get_length(input);
  if (*output) {
    evbuffer_drain(*output, evbuffer_get_length(*output));
  } else {
    if (!(*output = evbuffer_new()))
      return -1;
  }
  evbuffer_ptr_set((struct evbuffer*)input, &ptr, 0, EVBUFFER_PTR_SET);
  while (remaining) {
    struct evbuffer_iovec v[4];
    int n_used, i;
    n_used = evbuffer_peek((struct evbuffer*)input, -1, &ptr, v, 4);
    if (n_used < 0)
      return -1;
    for (i=0;i<n_used;++i) {
      evbuffer_add(*output, v[i].iov_base, v[i].iov_len);
      tor_assert(v[i].iov_len <= remaining);
      remaining -= v[i].iov_len;
      evbuffer_ptr_set((struct evbuffer*)input,
                       &ptr, v[i].iov_len, EVBUFFER_PTR_ADD);
    }
  }
#else
  if (*output)
    buf_free(*output);
  *output = buf_copy(input);
#endif
  return 0;
}