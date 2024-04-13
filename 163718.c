struct http_req_rule *parse_http_req_cond(const char **args, const char *file, int linenum, struct proxy *proxy)
{
	struct http_req_rule *rule;
	int cur_arg;

	rule = (struct http_req_rule*)calloc(1, sizeof(struct http_req_rule));
	if (!rule) {
		Alert("parsing [%s:%d]: out of memory.\n", file, linenum);
		goto out_err;
	}

	if (!strcmp(args[0], "allow")) {
		rule->action = HTTP_REQ_ACT_ALLOW;
		cur_arg = 1;
	} else if (!strcmp(args[0], "deny")) {
		rule->action = HTTP_REQ_ACT_DENY;
		cur_arg = 1;
	} else if (!strcmp(args[0], "tarpit")) {
		rule->action = HTTP_REQ_ACT_TARPIT;
		cur_arg = 1;
	} else if (!strcmp(args[0], "auth")) {
		rule->action = HTTP_REQ_ACT_AUTH;
		cur_arg = 1;

		while(*args[cur_arg]) {
			if (!strcmp(args[cur_arg], "realm")) {
				rule->arg.auth.realm = strdup(args[cur_arg + 1]);
				cur_arg+=2;
				continue;
			} else
				break;
		}
	} else if (strcmp(args[0], "add-header") == 0 || strcmp(args[0], "set-header") == 0) {
		rule->action = *args[0] == 'a' ? HTTP_REQ_ACT_ADD_HDR : HTTP_REQ_ACT_SET_HDR;
		cur_arg = 1;

		if (!*args[cur_arg] || !*args[cur_arg+1] || *args[cur_arg+2]) {
			Alert("parsing [%s:%d]: 'http-request %s' expects exactly 2 arguments.\n",
			      file, linenum, args[0]);
			goto out_err;
		}

		rule->arg.hdr_add.name = strdup(args[cur_arg]);
		rule->arg.hdr_add.name_len = strlen(rule->arg.hdr_add.name);
		LIST_INIT(&rule->arg.hdr_add.fmt);
		parse_logformat_string(args[cur_arg + 1], proxy, &rule->arg.hdr_add.fmt, 0);
		cur_arg += 2;
	} else if (strcmp(args[0], "redirect") == 0) {
		struct redirect_rule *redir;
		char *errmsg;

		if ((redir = http_parse_redirect_rule(file, linenum, proxy, (const char **)args + 1, &errmsg)) == NULL) {
			Alert("parsing [%s:%d] : error detected in %s '%s' while parsing 'http-request %s' rule : %s.\n",
			      file, linenum, proxy_type_str(proxy), proxy->id, args[0], errmsg);
			goto out_err;
		}

		/* this redirect rule might already contain a parsed condition which
		 * we'll pass to the http-request rule.
		 */
		rule->action = HTTP_REQ_ACT_REDIR;
		rule->arg.redir = redir;
		rule->cond = redir->cond;
		redir->cond = NULL;
		cur_arg = 2;
		return rule;
	} else {
		Alert("parsing [%s:%d]: 'http-request' expects 'allow', 'deny', 'auth', 'redirect', 'tarpit', 'add-header', 'set-header', but got '%s'%s.\n",
		      file, linenum, args[0], *args[0] ? "" : " (missing argument)");
		goto out_err;
	}

	if (strcmp(args[cur_arg], "if") == 0 || strcmp(args[cur_arg], "unless") == 0) {
		struct acl_cond *cond;
		char *errmsg = NULL;

		if ((cond = build_acl_cond(file, linenum, proxy, args+cur_arg, &errmsg)) == NULL) {
			Alert("parsing [%s:%d] : error detected while parsing an 'http-request %s' condition : %s.\n",
			      file, linenum, args[0], errmsg);
			free(errmsg);
			goto out_err;
		}
		rule->cond = cond;
	}
	else if (*args[cur_arg]) {
		Alert("parsing [%s:%d]: 'http-request %s' expects 'realm' for 'auth' or"
		      " either 'if' or 'unless' followed by a condition but found '%s'.\n",
		      file, linenum, args[0], args[cur_arg]);
		goto out_err;
	}

	return rule;
 out_err:
	free(rule);
	return NULL;
}