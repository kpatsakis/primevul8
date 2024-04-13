get_current_codepage(void)
{
	char *locale, *p;
	unsigned cp;

	locale = setlocale(LC_CTYPE, NULL);
	if (locale == NULL)
		return (GetACP());
	if (locale[0] == 'C' && locale[1] == '\0')
		return (CP_C_LOCALE);
	p = strrchr(locale, '.');
	if (p == NULL)
		return (GetACP());
	cp = my_atoi(p+1);
	if (cp <= 0)
		return (GetACP());
	return (cp);
}