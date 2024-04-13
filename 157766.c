buf_pullup(buf_t *buf, size_t bytes, int nulterminate)
{
  /* XXXX nothing uses nulterminate; remove it. */
  chunk_t *dest, *src;
  size_t capacity;
  if (!buf->head)
    return;

  check();
  if (buf->datalen < bytes)
    bytes = buf->datalen;

  if (nulterminate) {
    capacity = bytes + 1;
    if (buf->head->datalen >= bytes && CHUNK_REMAINING_CAPACITY(buf->head)) {
      *CHUNK_WRITE_PTR(buf->head) = '\0';
      return;
    }
  } else {
    capacity = bytes;
    if (buf->head->datalen >= bytes)
      return;
  }

  if (buf->head->memlen >= capacity) {
    /* We don't need to grow the first chunk, but we might need to repack it.*/
    size_t needed = capacity - buf->head->datalen;
    if (CHUNK_REMAINING_CAPACITY(buf->head) < needed)
      chunk_repack(buf->head);
    tor_assert(CHUNK_REMAINING_CAPACITY(buf->head) >= needed);
  } else {
    chunk_t *newhead;
    size_t newsize;
    /* We need to grow the chunk. */
    chunk_repack(buf->head);
    newsize = CHUNK_SIZE_WITH_ALLOC(preferred_chunk_size(capacity));
    newhead = chunk_grow(buf->head, newsize);
    tor_assert(newhead->memlen >= capacity);
    if (newhead != buf->head) {
      if (buf->tail == buf->head)
        buf->tail = newhead;
      buf->head = newhead;
    }
  }

  dest = buf->head;
  while (dest->datalen < bytes) {
    size_t n = bytes - dest->datalen;
    src = dest->next;
    tor_assert(src);
    if (n >= src->datalen) {
      memcpy(CHUNK_WRITE_PTR(dest), src->data, src->datalen);
      dest->datalen += src->datalen;
      dest->next = src->next;
      if (buf->tail == src)
        buf->tail = dest;
      chunk_free_unchecked(src);
    } else {
      memcpy(CHUNK_WRITE_PTR(dest), src->data, n);
      dest->datalen += n;
      src->data += n;
      src->datalen -= n;
      tor_assert(dest->datalen == bytes);
    }
  }

  if (nulterminate) {
    tor_assert(CHUNK_REMAINING_CAPACITY(buf->head));
    *CHUNK_WRITE_PTR(buf->head) = '\0';
  }

  check();
}