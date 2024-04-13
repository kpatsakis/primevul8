header_add_at_position(BOOL after, uschar *name, BOOL topnot, int type,
  const char *format, ...)
{
va_list ap;
va_start(ap, format);
header_add_backend(after, name, topnot, type, format, ap);
va_end(ap);
}