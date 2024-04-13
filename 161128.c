get_str_opt(struct archive_write *a, struct archive_string *s,
    size_t maxsize, const char *key, const char *value)
{

	if (strlen(value) > maxsize) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Value is longer than %zu characters "
		    "for option ``%s''", maxsize, key);
		return (ARCHIVE_FATAL);
	}
	archive_strcpy(s, value);
	return (ARCHIVE_OK);
}