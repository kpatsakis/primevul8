buf_pos_inc(buf_pos_t *pos)
{
  ++pos->pos;
  if (pos->pos == (off_t)pos->chunk->datalen) {
    if (!pos->chunk->next)
      return -1;
    pos->chunk_pos += pos->chunk->datalen;
    pos->chunk = pos->chunk->next;
    pos->pos = 0;
  }
  return 0;
}