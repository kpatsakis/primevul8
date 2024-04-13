char *LibRaw::strcasestr(char *haystack, const char *needle)
{
  char *c;
  for (c = haystack; *c; c++)
    if (!strncasecmp(c, needle, strlen(needle)))
      return c;
  return 0;
}