poolAppendString(STRING_POOL *pool, const XML_Char *s) {
  while (*s) {
    if (! poolAppendChar(pool, *s))
      return NULL;
    s++;
  }
  return pool->start;
}