static int fill_one(const char *what, struct match_attr *a, int rem)
{
	struct git_attr_check *check = check_all_attr;
	int i;

	for (i = 0; 0 < rem && i < a->num_attr; i++) {
		struct git_attr *attr = a->state[i].attr;
		const char **n = &(check[attr->attr_nr].value);
		const char *v = a->state[i].setto;

		if (*n == ATTR__UNKNOWN) {
			debug_set(what, a->u.pattern, attr, v);
			*n = v;
			rem--;
		}
	}
	return rem;
}