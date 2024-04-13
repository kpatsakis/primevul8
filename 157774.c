buf_get_first_chunk_data(const buf_t *buf, const char **cp, size_t *sz)
{
  if (!buf || !buf->head) {
    *cp = NULL;
    *sz = 0;
  } else {
    *cp = buf->head->data;
    *sz = buf->head->datalen;
  }
}