header_add(int type, const char *format, ...)
{
va_list ap;
va_start(ap, format);
header_add_backend(TRUE, NULL, FALSE, type, format, ap);
va_end(ap);
}