buf_new_with_capacity(size_t size)
{
  buf_t *b = buf_new();
  b->default_chunk_size = preferred_chunk_size(size);
  return b;
}