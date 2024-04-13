static void replace_char(char *string, char from, char to, int maxlen)
{
	char *lastchar = string + maxlen;
	while (string) {
		string = strchr(string, from);
		if (string) {
			*string = to;
			if (string >= lastchar)
				return;
		}
	}
}