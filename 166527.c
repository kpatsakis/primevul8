_PUBLIC_ int vfdprintf(int fd, const char *format, va_list ap)
{
	char *p;
	int len, ret;
	va_list ap2;

	va_copy(ap2, ap);
	len = vasprintf(&p, format, ap2);
	va_end(ap2);
	if (len <= 0) return len;
	ret = write(fd, p, len);
	SAFE_FREE(p);
	return ret;
}