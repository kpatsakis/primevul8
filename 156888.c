string_equal(const char *str1, const char *str2)
{
	if (!str1 && !str2)
		return true;
	if (!str1 != !str2)
		return false;

	return !strcmp(str1, str2);
}