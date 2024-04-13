static int invalid_attr_name(const char *name, int namelen)
{
	/*
	 * Attribute name cannot begin with '-' and from
	 * [-A-Za-z0-9_.].  We'd specifically exclude '=' for now,
	 * as we might later want to allow non-binary value for
	 * attributes, e.g. "*.svg	merge=special-merge-program-for-svg"
	 */
	if (*name == '-')
		return -1;
	while (namelen--) {
		char ch = *name++;
		if (! (ch == '-' || ch == '.' || ch == '_' ||
		       ('0' <= ch && ch <= '9') ||
		       ('a' <= ch && ch <= 'z') ||
		       ('A' <= ch && ch <= 'Z')) )
			return -1;
	}
	return 0;
}