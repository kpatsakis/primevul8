size_t LibRaw::strnlen(const char *s, size_t n)
{
#if !defined(__FreeBSD__) && !defined(__OpenBSD__)
  const char *p = (const char *)memchr(s, 0, n);
  return (p ? p - s : n);
#else
  return ::strnlen(s, n);
#endif
}