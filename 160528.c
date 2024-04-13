char *LibRaw_buffer_datastream::gets(char *s, int sz)
{
  unsigned char *psrc, *pdest, *str;
  str = (unsigned char *)s;
  psrc = buf + streampos;
  pdest = str;
  if(streampos >= streamsize) return NULL;
  while ((size_t(psrc - buf) < streamsize) && ((pdest - str) < sz))
  {
    *pdest = *psrc;
    if (*psrc == '\n')
      break;
    psrc++;
    pdest++;
  }
  if (size_t(psrc - buf) < streamsize)
    psrc++;
  if ((pdest - str) < sz)
    *(++pdest) = 0;
  streampos = psrc - buf;
  return s;
}