set_str_utf16be(struct archive_write *a, unsigned char *p, const char *s,
    size_t l, uint16_t uf, enum vdc vdc)
{
	size_t size, i;
	int onepad;

	if (s == NULL)
		s = "";
	if (l & 0x01) {
		onepad = 1;
		l &= ~1;
	} else
		onepad = 0;
	if (vdc == VDC_UCS2) {
		struct iso9660 *iso9660 = a->format_data;
		if (archive_strncpy_l(&iso9660->utf16be, s, strlen(s),
		    iso9660->sconv_to_utf16be) != 0 && errno == ENOMEM) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate memory for UTF-16BE");
			return (ARCHIVE_FATAL);
		}
		size = iso9660->utf16be.length;
		if (size > l)
			size = l;
		memcpy(p, iso9660->utf16be.s, size);
	} else {
		const uint16_t *u16 = (const uint16_t *)s;

		size = 0;
		while (*u16++)
			size += 2;
		if (size > l)
			size = l;
		memcpy(p, s, size);
	}
	for (i = 0; i < size; i += 2, p += 2) {
		if (!joliet_allowed_char(p[0], p[1]))
			archive_be16enc(p, 0x005F);/* '_' */
	}
	l -= size;
	while (l > 0) {
		archive_be16enc(p, uf);
		p += 2;
		l -= 2;
	}
	if (onepad)
		*p = 0;
	return (ARCHIVE_OK);
}