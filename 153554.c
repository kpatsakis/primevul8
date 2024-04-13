findAuthentication(struct http_auth *hauth, Buffer *buf, char *auth_field)
{
    struct http_auth *ha;
    int len = strlen(auth_field), slen;
    TextListItem *i;
    char *p0, *p;

    bzero(hauth, sizeof(struct http_auth));
    for (i = buf->document_header->first; i != NULL; i = i->next) {
	if (strncasecmp(i->ptr, auth_field, len) == 0) {
	    for (p = i->ptr + len; p != NULL && *p != '\0';) {
		SKIP_BLANKS(p);
		p0 = p;
		for (ha = &www_auth[0]; ha->scheme != NULL; ha++) {
		    slen = strlen(ha->scheme);
		    if (strncasecmp(p, ha->scheme, slen) == 0) {
			p += slen;
			SKIP_BLANKS(p);
			if (hauth->pri < ha->pri) {
			    *hauth = *ha;
			    p = extract_auth_param(p, hauth->param);
			    break;
			}
			else {
			    /* weak auth */
			    p = extract_auth_param(p, none_auth_param);
			}
		    }
		}
		if (p0 == p) {
		    /* all unknown auth failed */
		    int token_type;
		    if ((token_type = skip_auth_token(&p)) == AUTHCHR_TOKEN && IS_SPACE(*p)) {
			SKIP_BLANKS(p);
			p = extract_auth_param(p, none_auth_param);
		    }
		    else
			break;
		}
	    }
	}
    }
    return hauth->scheme ? hauth : NULL;
}