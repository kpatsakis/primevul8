static void split_certinfo(char *string, zval *hash)
{
	char *org = estrdup(string);
	char *s = org;
	char *split;

	if(org) {
		do {
			char *key;
			char *val;
			char *tmp;

			split = strstr(s, "; ");
			if(split)
				*split = '\0';

			key = s;
			tmp = memchr(key, '=', 64);
			if(tmp) {
				*tmp = '\0';
				val = tmp+1;
				add_assoc_string(hash, key, val, 1);
			}
			s = split+2;
		} while(split);
		efree(org);
	}
}