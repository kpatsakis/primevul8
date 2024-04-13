log_write(unsigned int selector, int flags, char *format, ...)
{
va_list ap;
va_start(ap, format);
vfprintf(stderr, format, ap);
fprintf(stderr, "\n");
va_end(ap);
selector = selector;     /* Keep picky compilers happy */
flags = flags;
}