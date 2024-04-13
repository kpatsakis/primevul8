poolCopyString(STRING_POOL *pool, const XML_Char *s) {
  do {
    if (! poolAppendChar(pool, *s))
      return NULL;
  } while (*s++);
  s = pool->start;
  poolFinish(pool);
  return s;
}