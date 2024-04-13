buf_find_offset_of_char(buf_t *buf, char ch)
{
  chunk_t *chunk;
  off_t offset = 0;
  for (chunk = buf->head; chunk; chunk = chunk->next) {
    char *cp = memchr(chunk->data, ch, chunk->datalen);
    if (cp)
      return offset + (cp - chunk->data);
    else
      offset += chunk->datalen;
  }
  return -1;
}