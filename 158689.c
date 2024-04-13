debug_printf(char *format, ...)
{
va_list ap;
gstring * g = string_get(1024);
void * reset_point = g;

va_start(ap, format);

if (!string_vformat(g, FALSE, format, ap))
  {
  char * s = "**** debug string overflowed buffer ****\n";
  char * p = CS g->s + g->ptr;
  int maxlen = g->size - (int)strlen(s) - 3;
  if (p > g->s + maxlen) p = g->s + maxlen;
  if (p > g->s && p[-1] != '\n') *p++ = '\n';
  strcpy(p, s);
  }

fprintf(stderr, "%s", string_from_gstring(g));
fflush(stderr);
store_reset(reset_point);
va_end(ap);
}