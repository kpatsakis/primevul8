void free_http_req_rules(struct list *r) {
	struct http_req_rule *tr, *pr;

	list_for_each_entry_safe(pr, tr, r, list) {
		LIST_DEL(&pr->list);
		if (pr->action == HTTP_REQ_ACT_AUTH)
			free(pr->arg.auth.realm);

		free(pr);
	}
}