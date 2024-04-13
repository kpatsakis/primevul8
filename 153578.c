process_input(struct parsed_tag *tag)
{
    int i = 20, v, x, y, z, iw, ih, size = 20;
    char *q, *p, *r, *p2, *s;
    Str tmp = NULL;
    char *qq = "";
    int qlen = 0;

    if (cur_form_id < 0) {
	char *s = "<form_int method=internal action=none>";
	tmp = process_form(parse_tag(&s, TRUE));
    }
    if (tmp == NULL)
	tmp = Strnew();

    p = "text";
    parsedtag_get_value(tag, ATTR_TYPE, &p);
    q = NULL;
    parsedtag_get_value(tag, ATTR_VALUE, &q);
    r = "";
    parsedtag_get_value(tag, ATTR_NAME, &r);
    parsedtag_get_value(tag, ATTR_SIZE, &size);
    if (size > MAX_INPUT_SIZE)
	    size = MAX_INPUT_SIZE;
    parsedtag_get_value(tag, ATTR_MAXLENGTH, &i);
    p2 = NULL;
    parsedtag_get_value(tag, ATTR_ALT, &p2);
    x = parsedtag_exists(tag, ATTR_CHECKED);
    y = parsedtag_exists(tag, ATTR_ACCEPT);
    z = parsedtag_exists(tag, ATTR_READONLY);

    v = formtype(p);
    if (v == FORM_UNKNOWN)
	return NULL;

    if (!q) {
	switch (v) {
	case FORM_INPUT_IMAGE:
	case FORM_INPUT_SUBMIT:
	case FORM_INPUT_BUTTON:
	    q = "SUBMIT";
	    break;
	case FORM_INPUT_RESET:
	    q = "RESET";
	    break;
	    /* if no VALUE attribute is specified in 
	     * <INPUT TYPE=CHECKBOX> tag, then the value "on" is used 
	     * as a default value. It is not a part of HTML4.0 
	     * specification, but an imitation of Netscape behaviour. 
	     */
	case FORM_INPUT_CHECKBOX:
	    q = "on";
	}
    }
    /* VALUE attribute is not allowed in <INPUT TYPE=FILE> tag. */
    if (v == FORM_INPUT_FILE)
	q = NULL;
    if (q) {
	qq = html_quote(q);
	qlen = get_strwidth(q);
    }

    Strcat_charp(tmp, "<pre_int>");
    switch (v) {
    case FORM_INPUT_PASSWORD:
    case FORM_INPUT_TEXT:
    case FORM_INPUT_FILE:
    case FORM_INPUT_CHECKBOX:
	if (displayLinkNumber)
	    Strcat(tmp, getLinkNumberStr(0));
	Strcat_char(tmp, '[');
	break;
    case FORM_INPUT_RADIO:
	if (displayLinkNumber)
	    Strcat(tmp, getLinkNumberStr(0));
	Strcat_char(tmp, '(');
    }
    Strcat(tmp, Sprintf("<input_alt hseq=\"%d\" fid=\"%d\" type=\"%s\" "
			"name=\"%s\" width=%d maxlength=%d value=\"%s\"",
			cur_hseq++, cur_form_id, html_quote(p),
			html_quote(r), size, i, qq));
    if (x)
	Strcat_charp(tmp, " checked");
    if (y)
	Strcat_charp(tmp, " accept");
    if (z)
	Strcat_charp(tmp, " readonly");
    Strcat_char(tmp, '>');

    if (v == FORM_INPUT_HIDDEN)
	Strcat_charp(tmp, "</input_alt></pre_int>");
    else {
	switch (v) {
	case FORM_INPUT_PASSWORD:
	case FORM_INPUT_TEXT:
	case FORM_INPUT_FILE:
	    Strcat_charp(tmp, "<u>");
	    break;
	case FORM_INPUT_IMAGE:
	    s = NULL;
	    parsedtag_get_value(tag, ATTR_SRC, &s);
	    if (s) {
		Strcat(tmp, Sprintf("<img src=\"%s\"", html_quote(s)));
		if (p2)
		    Strcat(tmp, Sprintf(" alt=\"%s\"", html_quote(p2)));
		if (parsedtag_get_value(tag, ATTR_WIDTH, &iw))
		    Strcat(tmp, Sprintf(" width=\"%d\"", iw));
		if (parsedtag_get_value(tag, ATTR_HEIGHT, &ih))
		    Strcat(tmp, Sprintf(" height=\"%d\"", ih));
		Strcat_charp(tmp, " pre_int>");
		Strcat_charp(tmp, "</input_alt></pre_int>");
		return tmp;
	    }
	case FORM_INPUT_SUBMIT:
	case FORM_INPUT_BUTTON:
	case FORM_INPUT_RESET:
	    if (displayLinkNumber)
		Strcat(tmp, getLinkNumberStr(-1));
	    Strcat_charp(tmp, "[");
	    break;
	}
	switch (v) {
	case FORM_INPUT_PASSWORD:
	    i = 0;
	    if (q) {
		for (; i < qlen && i < size; i++)
		    Strcat_char(tmp, '*');
	    }
	    for (; i < size; i++)
		Strcat_char(tmp, ' ');
	    break;
	case FORM_INPUT_TEXT:
	case FORM_INPUT_FILE:
	    if (q)
		Strcat(tmp, textfieldrep(Strnew_charp(q), size));
	    else {
		for (i = 0; i < size; i++)
		    Strcat_char(tmp, ' ');
	    }
	    break;
	case FORM_INPUT_SUBMIT:
	case FORM_INPUT_BUTTON:
	    if (p2)
		Strcat_charp(tmp, html_quote(p2));
	    else
		Strcat_charp(tmp, qq);
	    break;
	case FORM_INPUT_RESET:
	    Strcat_charp(tmp, qq);
	    break;
	case FORM_INPUT_RADIO:
	case FORM_INPUT_CHECKBOX:
	    if (x)
		Strcat_char(tmp, '*');
	    else
		Strcat_char(tmp, ' ');
	    break;
	}
	switch (v) {
	case FORM_INPUT_PASSWORD:
	case FORM_INPUT_TEXT:
	case FORM_INPUT_FILE:
	    Strcat_charp(tmp, "</u>");
	    break;
	case FORM_INPUT_IMAGE:
	case FORM_INPUT_SUBMIT:
	case FORM_INPUT_BUTTON:
	case FORM_INPUT_RESET:
	    Strcat_charp(tmp, "]");
	}
	Strcat_charp(tmp, "</input_alt>");
	switch (v) {
	case FORM_INPUT_PASSWORD:
	case FORM_INPUT_TEXT:
	case FORM_INPUT_FILE:
	case FORM_INPUT_CHECKBOX:
	    Strcat_char(tmp, ']');
	    break;
	case FORM_INPUT_RADIO:
	    Strcat_char(tmp, ')');
	}
	Strcat_charp(tmp, "</pre_int>");
    }
    return tmp;
}