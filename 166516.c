_PUBLIC_ int fdprintf(int fd, const char *format, ...)
{
	va_list ap;
	int ret;

	va_start(ap, format);
	ret = vfdprintf(fd, format, ap);
	va_end(ap);
	return ret;
}