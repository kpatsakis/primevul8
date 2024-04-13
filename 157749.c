chunk_free_unchecked(chunk_t *chunk)
{
  if (!chunk)
    return;
#ifdef DEBUG_CHUNK_ALLOC
  tor_assert(CHUNK_ALLOC_SIZE(chunk->memlen) == chunk->DBG_alloc);
#endif
  tor_assert(total_bytes_allocated_in_chunks >=
             CHUNK_ALLOC_SIZE(chunk->memlen));
  total_bytes_allocated_in_chunks -= CHUNK_ALLOC_SIZE(chunk->memlen);
  tor_free(chunk);
}