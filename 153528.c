skip_auth_token(char **pp)
{
    char *p;
    int first = AUTHCHR_NUL, typ;

    for (p = *pp ;; ++p) {
	switch (*p) {
	case '\0':
	    goto endoftoken;
	default:
	    if ((unsigned char)*p > 037) {
		typ = AUTHCHR_TOKEN;
		break;
	    }
	    /* thru */
	case '\177':
	case '[':
	case ']':
	case '(':
	case ')':
	case '<':
	case '>':
	case '@':
	case ';':
	case ':':
	case '\\':
	case '"':
	case '/':
	case '?':
	case '=':
	case ' ':
	case '\t':
	case ',':
	    typ = AUTHCHR_SEP;
	    break;
	}

	if (!first)
	    first = typ;
	else if (first != typ)
	    break;
    }
endoftoken:
    *pp = p;
    return first;
}