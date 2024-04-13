void *LibRaw::memmem(char *haystack, size_t haystacklen, char *needle,
                     size_t needlelen)
{
#if !defined(__GLIBC__) && !defined(__FreeBSD__) && !defined(__OpenBSD__)
  char *c;
  for (c = haystack; c <= haystack + haystacklen - needlelen; c++)
    if (!memcmp(c, needle, needlelen))
      return c;
  return 0;
#else
  return ::memmem(haystack, haystacklen, needle, needlelen);
#endif
}