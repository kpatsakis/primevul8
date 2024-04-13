buf_add_chunk_with_capacity(buf_t *buf, size_t capacity, int capped)
{
  chunk_t *chunk;
  struct timeval now;
  if (CHUNK_ALLOC_SIZE(capacity) < buf->default_chunk_size) {
    chunk = chunk_new_with_alloc_size(buf->default_chunk_size);
  } else if (capped && CHUNK_ALLOC_SIZE(capacity) > MAX_CHUNK_ALLOC) {
    chunk = chunk_new_with_alloc_size(MAX_CHUNK_ALLOC);
  } else {
    chunk = chunk_new_with_alloc_size(preferred_chunk_size(capacity));
  }

  tor_gettimeofday_cached_monotonic(&now);
  chunk->inserted_time = (uint32_t)tv_to_msec(&now);

  if (buf->tail) {
    tor_assert(buf->head);
    buf->tail->next = chunk;
    buf->tail = chunk;
  } else {
    tor_assert(!buf->head);
    buf->head = buf->tail = chunk;
  }
  check();
  return chunk;
}