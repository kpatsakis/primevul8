CHUNK_REMAINING_CAPACITY(const chunk_t *chunk)
{
  return (chunk->mem + chunk->memlen) - (chunk->data + chunk->datalen);
}