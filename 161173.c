get_num_opt(struct archive_write *a, int *num, int high, int low,
    const char *key, const char *value)
{
	const char *p = value;
	int data = 0;
	int neg = 0;

	if (p == NULL) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Invalid value(empty) for option ``%s''", key);
		return (ARCHIVE_FATAL);
	}
	if (*p == '-') {
		neg = 1;
		p++;
	}
	while (*p) {
		if (*p >= '0' && *p <= '9')
			data = data * 10 + *p - '0';
		else {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Invalid value for option ``%s''", key);
			return (ARCHIVE_FATAL);
		}
		if (data > high) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Invalid value(over %d) for "
			    "option ``%s''", high, key);
			return (ARCHIVE_FATAL);
		}
		if (data < low) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Invalid value(under %d) for "
			    "option ``%s''", low, key);
			return (ARCHIVE_FATAL);
		}
		p++;
	}
	if (neg)
		data *= -1;
	*num = data;

	return (ARCHIVE_OK);
}