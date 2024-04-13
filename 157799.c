buf_slack(const buf_t *buf)
{
  if (!buf->tail)
    return 0;
  else
    return CHUNK_REMAINING_CAPACITY(buf->tail);
}