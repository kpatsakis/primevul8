cleanup_backslash_2(wchar_t *p)
{

	/* Convert a path-separator from '\' to  '/' */
	while (*p != L'\0') {
		if (*p == L'\\')
			*p = L'/';
		p++;
	}
}