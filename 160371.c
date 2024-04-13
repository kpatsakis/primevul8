int LibRaw_buffer_datastream::read(void *ptr, size_t sz, size_t nmemb)
{
  size_t to_read = sz * nmemb;
  if (to_read > streamsize - streampos)
    to_read = streamsize - streampos;
  if (to_read < 1)
    return 0;
  memmove(ptr, buf + streampos, to_read);
  streampos += to_read;
  return int((to_read + sz - 1) / (sz > 0 ? sz : 1));
}