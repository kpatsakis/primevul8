fetch_from_buf_line(buf_t *buf, char *data_out, size_t *data_len)
{
  size_t sz;
  off_t offset;

  if (!buf->head)
    return 0;

  offset = buf_find_offset_of_char(buf, '\n');
  if (offset < 0)
    return 0;
  sz = (size_t) offset;
  if (sz+2 > *data_len) {
    *data_len = sz + 2;
    return -1;
  }
  fetch_from_buf(data_out, sz+1, buf);
  data_out[sz+1] = '\0';
  *data_len = sz+1;
  return 1;
}