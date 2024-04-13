extract_auth_val(char **q)
{
    unsigned char *qq = *(unsigned char **)q;
    int quoted = 0;
    Str val = Strnew();

    SKIP_BLANKS(qq);
    if (*qq == '"') {
	quoted = TRUE;
	Strcat_char(val, *qq++);
    }
    while (*qq != '\0') {
	if (quoted && *qq == '"') {
	    Strcat_char(val, *qq++);
	    break;
	}
	if (!quoted) {
	    switch (*qq) {
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
		qq++;
	    case ',':
		goto end_token;
	    default:
		if (*qq <= 037 || *qq == 0177) {
		    qq++;
		    goto end_token;
		}
	    }
	}
	else if (quoted && *qq == '\\')
	    Strcat_char(val, *qq++);
	Strcat_char(val, *qq++);
    }
  end_token:
    *q = (char *)qq;
    return val;
}