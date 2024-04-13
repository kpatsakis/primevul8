lha_parse_linkname(struct archive_wstring *linkname,
    struct archive_wstring *pathname)
{
	wchar_t *	linkptr;
	size_t 	symlen;

	linkptr = wcschr(pathname->s, L'|');
	if (linkptr != NULL) {
		symlen = wcslen(linkptr + 1);
		archive_wstrncpy(linkname, linkptr+1, symlen);

		*linkptr = 0;
		pathname->length = wcslen(pathname->s);

		return (1);
	}
	return (0);
}