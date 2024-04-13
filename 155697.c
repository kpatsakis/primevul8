normalizeLines(XML_Char *s) {
  XML_Char *p;
  for (;; s++) {
    if (*s == XML_T('\0'))
      return;
    if (*s == 0xD)
      break;
  }
  p = s;
  do {
    if (*s == 0xD) {
      *p++ = 0xA;
      if (*++s == 0xA)
        s++;
    } else
      *p++ = *s++;
  } while (*s);
  *p = XML_T('\0');
}