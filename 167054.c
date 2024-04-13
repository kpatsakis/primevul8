dot_to_underscore(gchar* str)
{
	unsigned i;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] == '.')
			str[i] = '_';
	}
	return str;
}