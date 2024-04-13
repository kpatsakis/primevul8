ftrace_function_filter_re(char *buf, int len, int *count)
{
	char *str, **re;

	str = kstrndup(buf, len, GFP_KERNEL);
	if (!str)
		return NULL;

	/*
	 * The argv_split function takes white space
	 * as a separator, so convert ',' into spaces.
	 */
	strreplace(str, ',', ' ');

	re = argv_split(GFP_KERNEL, str, count);
	kfree(str);
	return re;
}