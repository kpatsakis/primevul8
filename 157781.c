buf_copy(const buf_t *buf)
{
  chunk_t *ch;
  buf_t *out = buf_new();
  out->default_chunk_size = buf->default_chunk_size;
  for (ch = buf->head; ch; ch = ch->next) {
    chunk_t *newch = chunk_copy(ch);
    if (out->tail) {
      out->tail->next = newch;
      out->tail = newch;
    } else {
      out->head = out->tail = newch;
    }
  }
  out->datalen = buf->datalen;
  return out;
}