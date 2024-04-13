debug_printf(const char *format, ...)
{
va_list ap;
va_start(ap, format);
debug_vprintf(0, format, ap);
va_end(ap);
}