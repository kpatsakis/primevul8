struct redirect_rule *http_parse_redirect_rule(const char *file, int linenum, struct proxy *curproxy,
                                               const char **args, char **errmsg)
{
	struct redirect_rule *rule;
	int cur_arg;
	int type = REDIRECT_TYPE_NONE;
	int code = 302;
	const char *destination = NULL;
	const char *cookie = NULL;
	int cookie_set = 0;
	unsigned int flags = REDIRECT_FLAG_NONE;
	struct acl_cond *cond = NULL;

	cur_arg = 0;
	while (*(args[cur_arg])) {
		if (strcmp(args[cur_arg], "location") == 0) {
			if (!*args[cur_arg + 1])
				goto missing_arg;

			type = REDIRECT_TYPE_LOCATION;
			cur_arg++;
			destination = args[cur_arg];
		}
		else if (strcmp(args[cur_arg], "prefix") == 0) {
			if (!*args[cur_arg + 1])
				goto missing_arg;

			type = REDIRECT_TYPE_PREFIX;
			cur_arg++;
			destination = args[cur_arg];
		}
		else if (strcmp(args[cur_arg], "scheme") == 0) {
			if (!*args[cur_arg + 1])
				goto missing_arg;

			type = REDIRECT_TYPE_SCHEME;
			cur_arg++;
			destination = args[cur_arg];
		}
		else if (strcmp(args[cur_arg], "set-cookie") == 0) {
			if (!*args[cur_arg + 1])
				goto missing_arg;

			cur_arg++;
			cookie = args[cur_arg];
			cookie_set = 1;
		}
		else if (strcmp(args[cur_arg], "clear-cookie") == 0) {
			if (!*args[cur_arg + 1])
				goto missing_arg;

			cur_arg++;
			cookie = args[cur_arg];
			cookie_set = 0;
		}
		else if (strcmp(args[cur_arg], "code") == 0) {
			if (!*args[cur_arg + 1])
				goto missing_arg;

			cur_arg++;
			code = atol(args[cur_arg]);
			if (code < 301 || code > 308 || (code > 303 && code < 307)) {
				memprintf(errmsg,
				          "'%s': unsupported HTTP code '%s' (must be one of 301, 302, 303, 307 or 308)",
				          args[cur_arg - 1], args[cur_arg]);
				return NULL;
			}
		}
		else if (!strcmp(args[cur_arg],"drop-query")) {
			flags |= REDIRECT_FLAG_DROP_QS;
		}
		else if (!strcmp(args[cur_arg],"append-slash")) {
			flags |= REDIRECT_FLAG_APPEND_SLASH;
		}
		else if (strcmp(args[cur_arg], "if") == 0 ||
			 strcmp(args[cur_arg], "unless") == 0) {
			cond = build_acl_cond(file, linenum, curproxy, (const char **)args + cur_arg, errmsg);
			if (!cond) {
				memprintf(errmsg, "error in condition: %s", *errmsg);
				return NULL;
			}
			break;
		}
		else {
			memprintf(errmsg,
			          "expects 'code', 'prefix', 'location', 'scheme', 'set-cookie', 'clear-cookie', 'drop-query' or 'append-slash' (was '%s')",
			          args[cur_arg]);
			return NULL;
		}
		cur_arg++;
	}

	if (type == REDIRECT_TYPE_NONE) {
		memprintf(errmsg, "redirection type expected ('prefix', 'location', or 'scheme')");
		return NULL;
	}

	rule = (struct redirect_rule *)calloc(1, sizeof(*rule));
	rule->cond = cond;
	rule->rdr_str = strdup(destination);
	rule->rdr_len = strlen(destination);
	if (cookie) {
		/* depending on cookie_set, either we want to set the cookie, or to clear it.
		 * a clear consists in appending "; path=/; Max-Age=0;" at the end.
		 */
		rule->cookie_len = strlen(cookie);
		if (cookie_set) {
			rule->cookie_str = malloc(rule->cookie_len + 10);
			memcpy(rule->cookie_str, cookie, rule->cookie_len);
			memcpy(rule->cookie_str + rule->cookie_len, "; path=/;", 10);
			rule->cookie_len += 9;
		} else {
			rule->cookie_str = malloc(rule->cookie_len + 21);
			memcpy(rule->cookie_str, cookie, rule->cookie_len);
			memcpy(rule->cookie_str + rule->cookie_len, "; path=/; Max-Age=0;", 21);
			rule->cookie_len += 20;
		}
	}
	rule->type = type;
	rule->code = code;
	rule->flags = flags;
	LIST_INIT(&rule->list);
	return rule;

 missing_arg:
	memprintf(errmsg, "missing argument for '%s'", args[cur_arg]);
	return NULL;
}