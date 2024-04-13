static struct attr_stack *read_attr(const char *path, int macro_ok)
{
	struct attr_stack *res;
	char *buf, *sp;
	int lineno = 0;

	res = read_attr_from_file(path, macro_ok);
	if (res)
		return res;

	res = xcalloc(1, sizeof(*res));

	/*
	 * There is no checked out .gitattributes file there, but
	 * we might have it in the index.  We allow operation in a
	 * sparsely checked out work tree, so read from it.
	 */
	buf = read_index_data(path);
	if (!buf)
		return res;

	for (sp = buf; *sp; ) {
		char *ep;
		int more;
		for (ep = sp; *ep && *ep != '\n'; ep++)
			;
		more = (*ep == '\n');
		*ep = '\0';
		handle_attr_line(res, sp, path, ++lineno, macro_ok);
		sp = ep + more;
	}
	free(buf);
	return res;
}