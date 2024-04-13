char *find_hdr_value_end(char *s, const char *e)
{
	int quoted, qdpair;

	quoted = qdpair = 0;
	for (; s < e; s++) {
		if (qdpair)                    qdpair = 0;
		else if (quoted) {
			if (*s == '\\')        qdpair = 1;
			else if (*s == '"')    quoted = 0;
		}
		else if (*s == '"')            quoted = 1;
		else if (*s == ',')            return s;
	}
	return s;
}