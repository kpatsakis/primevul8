static struct attr_stack *read_attr_from_array(const char **list)
{
	struct attr_stack *res;
	const char *line;
	int lineno = 0;

	res = xcalloc(1, sizeof(*res));
	while ((line = *(list++)) != NULL)
		handle_attr_line(res, line, "[builtin]", ++lineno, 1);
	return res;
}