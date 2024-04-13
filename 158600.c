smtp_printf(const char *format, BOOL more, ...)
{
va_list ap;

va_start(ap, more);
smtp_vprintf(format, more, ap);
va_end(ap);
}