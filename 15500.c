str_append(char *dst, int len, const char *src)
{
  while ((len-- > 0) && *src)
    *dst++ = *src++;
  return dst;
}