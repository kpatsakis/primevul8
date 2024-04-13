keylen(KEY s) {
  size_t len = 0;
  for (; *s; s++, len++)
    ;
  return len;
}