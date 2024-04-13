static const char *parse_attr(const char *src, int lineno, const char *cp,
			      int *num_attr, struct match_attr *res)
{
	const char *ep, *equals;
	int len;

	ep = cp + strcspn(cp, blank);
	equals = strchr(cp, '=');
	if (equals && ep < equals)
		equals = NULL;
	if (equals)
		len = equals - cp;
	else
		len = ep - cp;
	if (!res) {
		if (*cp == '-' || *cp == '!') {
			cp++;
			len--;
		}
		if (invalid_attr_name(cp, len)) {
			fprintf(stderr,
				"%.*s is not a valid attribute name: %s:%d\n",
				len, cp, src, lineno);
			return NULL;
		}
	} else {
		struct attr_state *e;

		e = &(res->state[*num_attr]);
		if (*cp == '-' || *cp == '!') {
			e->setto = (*cp == '-') ? ATTR__FALSE : ATTR__UNSET;
			cp++;
			len--;
		}
		else if (!equals)
			e->setto = ATTR__TRUE;
		else {
			e->setto = xmemdupz(equals + 1, ep - equals - 1);
		}
		e->attr = git_attr(cp, len);
	}
	(*num_attr)++;
	return ep + strspn(ep, blank);
}