buf_find_pos_of_char(char ch, buf_pos_t *out)
{
  const chunk_t *chunk;
  int pos;
  tor_assert(out);
  if (out->chunk) {
    if (out->chunk->datalen) {
      tor_assert(out->pos < (off_t)out->chunk->datalen);
    } else {
      tor_assert(out->pos == 0);
    }
  }
  pos = out->pos;
  for (chunk = out->chunk; chunk; chunk = chunk->next) {
    char *cp = memchr(chunk->data+pos, ch, chunk->datalen - pos);
    if (cp) {
      out->chunk = chunk;
      tor_assert(cp - chunk->data < INT_MAX);
      out->pos = (int)(cp - chunk->data);
      return out->chunk_pos + out->pos;
    } else {
      out->chunk_pos += chunk->datalen;
      pos = 0;
    }
  }
  return -1;
}