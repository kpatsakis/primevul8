buf_get_oldest_chunk_timestamp(const buf_t *buf, uint32_t now)
{
  if (buf->head) {
    return now - buf->head->inserted_time;
  } else {
    return 0;
  }
}