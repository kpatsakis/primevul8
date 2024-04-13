set_option_info(struct archive_string *info, int *opt, const char *key,
    enum keytype type,  ...)
{
	va_list ap;
	char prefix;
	const char *s;
	int d;

	prefix = (*opt==0)? ' ':',';
	va_start(ap, type);
	switch (type) {
	case KEY_FLG:
		d = va_arg(ap, int);
		archive_string_sprintf(info, "%c%s%s",
		    prefix, (d == 0)?"!":"", key);
		break;
	case KEY_STR:
		s = va_arg(ap, const char *);
		archive_string_sprintf(info, "%c%s=%s",
		    prefix, key, s);
		break;
	case KEY_INT:
		d = va_arg(ap, int);
		archive_string_sprintf(info, "%c%s=%d",
		    prefix, key, d);
		break;
	case KEY_HEX:
		d = va_arg(ap, int);
		archive_string_sprintf(info, "%c%s=%x",
		    prefix, key, d);
		break;
	}
	va_end(ap);

	*opt = 1;
}