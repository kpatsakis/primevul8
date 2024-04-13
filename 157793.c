CHUNK_WRITE_PTR(chunk_t *chunk)
{
  return chunk->data + chunk->datalen;
}