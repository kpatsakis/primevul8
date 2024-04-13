exim_fail(const char * fmt, ...)
{
va_list ap;
va_start(ap, fmt);
vfprintf(stderr, fmt, ap);
exit(EXIT_FAILURE);
}