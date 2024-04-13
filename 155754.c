keyeq(KEY s1, KEY s2) {
  for (; *s1 == *s2; s1++, s2++)
    if (*s1 == 0)
      return XML_TRUE;
  return XML_FALSE;
}