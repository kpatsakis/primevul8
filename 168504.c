static void bootstrap_attr_stack(void)
{
	if (!attr_stack) {
		struct attr_stack *elem;

		elem = read_attr_from_array(builtin_attr);
		elem->origin = NULL;
		elem->prev = attr_stack;
		attr_stack = elem;

		if (!is_bare_repository()) {
			elem = read_attr(GITATTRIBUTES_FILE, 1);
			elem->origin = strdup("");
			elem->prev = attr_stack;
			attr_stack = elem;
			debug_push(elem);
		}

		elem = read_attr_from_file(git_path(INFOATTRIBUTES_FILE), 1);
		if (!elem)
			elem = xcalloc(1, sizeof(*elem));
		elem->origin = NULL;
		elem->prev = attr_stack;
		attr_stack = elem;
	}
}