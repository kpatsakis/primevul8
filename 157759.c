preferred_chunk_size(size_t target)
{
  size_t sz = MIN_CHUNK_ALLOC;
  while (CHUNK_SIZE_WITH_ALLOC(sz) < target) {
    sz <<= 1;
  }
  return sz;
}