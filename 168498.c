int git_checkattr(const char *path, int num, struct git_attr_check *check)
{
	struct attr_stack *stk;
	const char *cp;
	int dirlen, pathlen, i, rem;

	bootstrap_attr_stack();
	for (i = 0; i < attr_nr; i++)
		check_all_attr[i].value = ATTR__UNKNOWN;

	pathlen = strlen(path);
	cp = strrchr(path, '/');
	if (!cp)
		dirlen = 0;
	else
		dirlen = cp - path;
	prepare_attr_stack(path, dirlen);
	rem = attr_nr;
	for (stk = attr_stack; 0 < rem && stk; stk = stk->prev)
		rem = fill(path, pathlen, stk, rem);

	for (stk = attr_stack; 0 < rem && stk; stk = stk->prev)
		rem = macroexpand(stk, rem);

	for (i = 0; i < num; i++) {
		const char *value = check_all_attr[check[i].attr->attr_nr].value;
		if (value == ATTR__UNKNOWN)
			value = ATTR__UNSET;
		check[i].value = value;
	}

	return 0;
}