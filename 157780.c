peek_from_buf(char *string, size_t string_len, const buf_t *buf)
{
  chunk_t *chunk;

  tor_assert(string);
  /* make sure we don't ask for too much */
  tor_assert(string_len <= buf->datalen);
  /* assert_buf_ok(buf); */

  chunk = buf->head;
  while (string_len) {
    size_t copy = string_len;
    tor_assert(chunk);
    if (chunk->datalen < copy)
      copy = chunk->datalen;
    memcpy(string, chunk->data, copy);
    string_len -= copy;
    string += copy;
    chunk = chunk->next;
  }
}