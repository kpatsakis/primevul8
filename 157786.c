buf_allocation(const buf_t *buf)
{
  size_t total = 0;
  const chunk_t *chunk;
  for (chunk = buf->head; chunk; chunk = chunk->next) {
    total += CHUNK_ALLOC_SIZE(chunk->memlen);
  }
  return total;
}