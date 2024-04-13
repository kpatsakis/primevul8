static int macroexpand(struct attr_stack *stk, int rem)
{
	int i;
	struct git_attr_check *check = check_all_attr;

	for (i = stk->num_matches - 1; 0 < rem && 0 <= i; i--) {
		struct match_attr *a = stk->attrs[i];
		if (!a->is_macro)
			continue;
		if (check[a->u.attr->attr_nr].value != ATTR__TRUE)
			continue;
		rem = fill_one("expand", a, rem);
	}
	return rem;
}