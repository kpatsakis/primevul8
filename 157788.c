buf_clear(buf_t *buf)
{
  chunk_t *chunk, *next;
  buf->datalen = 0;
  for (chunk = buf->head; chunk; chunk = next) {
    next = chunk->next;
    chunk_free_unchecked(chunk);
  }
  buf->head = buf->tail = NULL;
}