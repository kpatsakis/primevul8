static void debug_set(const char *what, const char *match, struct git_attr *attr, const void *v)
{
	const char *value = v;

	if (ATTR_TRUE(value))
		value = "set";
	else if (ATTR_FALSE(value))
		value = "unset";
	else if (ATTR_UNSET(value))
		value = "unspecified";

	fprintf(stderr, "%s: %s => %s (%s)\n",
		what, attr->name, (char *) value, match);
}