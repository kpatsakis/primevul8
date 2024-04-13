chunk_copy(const chunk_t *in_chunk)
{
  chunk_t *newch = tor_memdup(in_chunk, CHUNK_ALLOC_SIZE(in_chunk->memlen));
  total_bytes_allocated_in_chunks += CHUNK_ALLOC_SIZE(in_chunk->memlen);
#ifdef DEBUG_CHUNK_ALLOC
  newch->DBG_alloc = CHUNK_ALLOC_SIZE(in_chunk->memlen);
#endif
  newch->next = NULL;
  if (in_chunk->data) {
    off_t offset = in_chunk->data - in_chunk->mem;
    newch->data = newch->mem + offset;
  }
  return newch;
}