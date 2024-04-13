static struct match_attr *parse_attr_line(const char *line, const char *src,
					  int lineno, int macro_ok)
{
	int namelen;
	int num_attr;
	const char *cp, *name;
	struct match_attr *res = NULL;
	int pass;
	int is_macro;

	cp = line + strspn(line, blank);
	if (!*cp || *cp == '#')
		return NULL;
	name = cp;
	namelen = strcspn(name, blank);
	if (strlen(ATTRIBUTE_MACRO_PREFIX) < namelen &&
	    !prefixcmp(name, ATTRIBUTE_MACRO_PREFIX)) {
		if (!macro_ok) {
			fprintf(stderr, "%s not allowed: %s:%d\n",
				name, src, lineno);
			return NULL;
		}
		is_macro = 1;
		name += strlen(ATTRIBUTE_MACRO_PREFIX);
		name += strspn(name, blank);
		namelen = strcspn(name, blank);
		if (invalid_attr_name(name, namelen)) {
			fprintf(stderr,
				"%.*s is not a valid attribute name: %s:%d\n",
				namelen, name, src, lineno);
			return NULL;
		}
	}
	else
		is_macro = 0;

	for (pass = 0; pass < 2; pass++) {
		/* pass 0 counts and allocates, pass 1 fills */
		num_attr = 0;
		cp = name + namelen;
		cp = cp + strspn(cp, blank);
		while (*cp) {
			cp = parse_attr(src, lineno, cp, &num_attr, res);
			if (!cp)
				return NULL;
		}
		if (pass)
			break;
		res = xcalloc(1,
			      sizeof(*res) +
			      sizeof(struct attr_state) * num_attr +
			      (is_macro ? 0 : namelen + 1));
		if (is_macro)
			res->u.attr = git_attr(name, namelen);
		else {
			res->u.pattern = (char*)&(res->state[num_attr]);
			memcpy(res->u.pattern, name, namelen);
			res->u.pattern[namelen] = 0;
		}
		res->is_macro = is_macro;
		res->num_attr = num_attr;
	}
	return res;
}