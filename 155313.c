static int cmm_skip_blanks(char *cp, char **endp)
{
	char *str;

	for (str = cp; *str == ' ' || *str == '\t'; str++)
		;
	*endp = str;
	return str != cp;
}