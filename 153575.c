extract_auth_param(char *q, struct auth_param *auth)
{
    struct auth_param *ap;
    char *p;

    for (ap = auth; ap->name != NULL; ap++) {
	ap->val = NULL;
    }

    while (*q != '\0') {
	SKIP_BLANKS(q);
	for (ap = auth; ap->name != NULL; ap++) {
	    size_t len;

	    len = strlen(ap->name);
	    if (strncasecmp(q, ap->name, len) == 0 &&
		(IS_SPACE(q[len]) || q[len] == '=')) {
		p = q + len;
		SKIP_BLANKS(p);
		if (*p != '=')
		    return q;
		q = p + 1;
		ap->val = extract_auth_val(&q);
		break;
	    }
	}
	if (ap->name == NULL) {
	    /* skip unknown param */
	    int token_type;
	    p = q;
	    if ((token_type = skip_auth_token(&q)) == AUTHCHR_TOKEN &&
		(IS_SPACE(*q) || *q == '=')) {
		SKIP_BLANKS(q);
		if (*q != '=')
		    return p;
		q++;
		extract_auth_val(&q);
	    }
	    else
		return p;
	}
	if (*q != '\0') {
	    SKIP_BLANKS(q);
	    if (*q == ',')
		q++;
	    else
		break;
	}
    }
    return q;
}