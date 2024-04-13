p_string(FILE* f, int len, UChar* s)
{
  fputs(":", f);
  while (len-- > 0) { fputc(*s++, f); }
}