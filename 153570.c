readHeader(URLFile *uf, Buffer *newBuf, int thru, ParsedURL *pu)
{
    char *p, *q;
#ifdef USE_COOKIE
    char *emsg;
#endif
    char c;
    Str lineBuf2 = NULL;
    Str tmp;
    TextList *headerlist;
#ifdef USE_M17N
    wc_ces charset = WC_CES_US_ASCII, mime_charset;
#endif
    char *tmpf;
    FILE *src = NULL;
    Lineprop *propBuffer;

    headerlist = newBuf->document_header = newTextList();
    if (uf->scheme == SCM_HTTP
#ifdef USE_SSL
	|| uf->scheme == SCM_HTTPS
#endif				/* USE_SSL */
	)
	http_response_code = -1;
    else
	http_response_code = 0;

    if (thru && !newBuf->header_source
#ifdef USE_IMAGE
	&& !image_source
#endif
	) {
	tmpf = tmpfname(TMPF_DFL, NULL)->ptr;
	src = fopen(tmpf, "w");
	if (src)
	    newBuf->header_source = tmpf;
    }
    while ((tmp = StrmyUFgets(uf))->length) {
#ifdef USE_NNTP
	if (uf->scheme == SCM_NEWS && tmp->ptr[0] == '.')
	    Strshrinkfirst(tmp, 1);
#endif
	if(w3m_reqlog){
	    FILE *ff;
	    ff = fopen(w3m_reqlog, "a");
	    Strfputs(tmp, ff);
	    fclose(ff);
	}
	if (src)
	    Strfputs(tmp, src);
	cleanup_line(tmp, HEADER_MODE);
	if (tmp->ptr[0] == '\n' || tmp->ptr[0] == '\r' || tmp->ptr[0] == '\0') {
	    if (!lineBuf2)
		/* there is no header */
		break;
	    /* last header */
	}
	else if (!(w3m_dump & DUMP_HEAD)) {
	    if (lineBuf2) {
		Strcat(lineBuf2, tmp);
	    }
	    else {
		lineBuf2 = tmp;
	    }
	    c = UFgetc(uf);
	    UFundogetc(uf);
	    if (c == ' ' || c == '\t')
		/* header line is continued */
		continue;
	    lineBuf2 = decodeMIME(lineBuf2, &mime_charset);
	    lineBuf2 = convertLine(NULL, lineBuf2, RAW_MODE,
				   mime_charset ? &mime_charset : &charset,
				   mime_charset ? mime_charset
				   : DocumentCharset);
	    /* separated with line and stored */
	    tmp = Strnew_size(lineBuf2->length);
	    for (p = lineBuf2->ptr; *p; p = q) {
		for (q = p; *q && *q != '\r' && *q != '\n'; q++) ;
		lineBuf2 = checkType(Strnew_charp_n(p, q - p), &propBuffer,
				     NULL);
		Strcat(tmp, lineBuf2);
		if (thru)
		    addnewline(newBuf, lineBuf2->ptr, propBuffer, NULL,
			       lineBuf2->length, FOLD_BUFFER_WIDTH, -1);
		for (; *q && (*q == '\r' || *q == '\n'); q++) ;
	    }
#ifdef USE_IMAGE
	    if (thru && activeImage && displayImage) {
		Str src = NULL;
		if (!strncasecmp(tmp->ptr, "X-Image-URL:", 12)) {
		    tmpf = &tmp->ptr[12];
		    SKIP_BLANKS(tmpf);
		    src = Strnew_m_charp("<img src=\"", html_quote(tmpf),
					 "\" alt=\"X-Image-URL\">", NULL);
		}
#ifdef USE_XFACE
		else if (!strncasecmp(tmp->ptr, "X-Face:", 7)) {
		    tmpf = xface2xpm(&tmp->ptr[7]);
		    if (tmpf)
			src = Strnew_m_charp("<img src=\"file:",
					     html_quote(tmpf),
					     "\" alt=\"X-Face\"",
					     " width=48 height=48>", NULL);
		}
#endif
		if (src) {
		    URLFile f;
		    Line *l;
#ifdef USE_M17N
		    wc_ces old_charset = newBuf->document_charset;
#endif
		    init_stream(&f, SCM_LOCAL, newStrStream(src));
		    loadHTMLstream(&f, newBuf, NULL, TRUE);
		    UFclose(&f);
		    for (l = newBuf->lastLine; l && l->real_linenumber;
			 l = l->prev)
			l->real_linenumber = 0;
#ifdef USE_M17N
		    newBuf->document_charset = old_charset;
#endif
		}
	    }
#endif
	    lineBuf2 = tmp;
	}
	else {
	    lineBuf2 = tmp;
	}
	if ((uf->scheme == SCM_HTTP
#ifdef USE_SSL
	     || uf->scheme == SCM_HTTPS
#endif				/* USE_SSL */
	    ) && http_response_code == -1) {
	    p = lineBuf2->ptr;
	    while (*p && !IS_SPACE(*p))
		p++;
	    while (*p && IS_SPACE(*p))
		p++;
	    http_response_code = atoi(p);
	    if (fmInitialized) {
		message(lineBuf2->ptr, 0, 0);
		refresh();
	    }
	}
	if (!strncasecmp(lineBuf2->ptr, "content-transfer-encoding:", 26)) {
	    p = lineBuf2->ptr + 26;
	    while (IS_SPACE(*p))
		p++;
	    if (!strncasecmp(p, "base64", 6))
		uf->encoding = ENC_BASE64;
	    else if (!strncasecmp(p, "quoted-printable", 16))
		uf->encoding = ENC_QUOTE;
	    else if (!strncasecmp(p, "uuencode", 8) ||
		     !strncasecmp(p, "x-uuencode", 10))
		uf->encoding = ENC_UUENCODE;
	    else
		uf->encoding = ENC_7BIT;
	}
	else if (!strncasecmp(lineBuf2->ptr, "content-encoding:", 17)) {
	    struct compression_decoder *d;
	    p = lineBuf2->ptr + 17;
	    while (IS_SPACE(*p))
		p++;
	    uf->compression = CMP_NOCOMPRESS;
	    for (d = compression_decoders; d->type != CMP_NOCOMPRESS; d++) {
		char **e;
		for (e = d->encodings; *e != NULL; e++) {
		    if (strncasecmp(p, *e, strlen(*e)) == 0) {
			uf->compression = d->type;
			break;
		    }
		}
		if (uf->compression != CMP_NOCOMPRESS)
		    break;
	    }
	    uf->content_encoding = uf->compression;
	}
#ifdef USE_COOKIE
	else if (use_cookie && accept_cookie &&
		 pu && check_cookie_accept_domain(pu->host) &&
		 (!strncasecmp(lineBuf2->ptr, "Set-Cookie:", 11) ||
		  !strncasecmp(lineBuf2->ptr, "Set-Cookie2:", 12))) {
	    Str name = Strnew(), value = Strnew(), domain = NULL, path = NULL,
		comment = NULL, commentURL = NULL, port = NULL, tmp2;
	    int version, quoted, flag = 0;
	    time_t expires = (time_t) - 1;

	    q = NULL;
	    if (lineBuf2->ptr[10] == '2') {
		p = lineBuf2->ptr + 12;
		version = 1;
	    }
	    else {
		p = lineBuf2->ptr + 11;
		version = 0;
	    }
#ifdef DEBUG
	    fprintf(stderr, "Set-Cookie: [%s]\n", p);
#endif				/* DEBUG */
	    SKIP_BLANKS(p);
	    while (*p != '=' && !IS_ENDT(*p))
		Strcat_char(name, *(p++));
	    Strremovetrailingspaces(name);
	    if (*p == '=') {
		p++;
		SKIP_BLANKS(p);
		quoted = 0;
		while (!IS_ENDL(*p) && (quoted || *p != ';')) {
		    if (!IS_SPACE(*p))
			q = p;
		    if (*p == '"')
			quoted = (quoted) ? 0 : 1;
		    Strcat_char(value, *(p++));
		}
		if (q)
		    Strshrink(value, p - q - 1);
	    }
	    while (*p == ';') {
		p++;
		SKIP_BLANKS(p);
		if (matchattr(p, "expires", 7, &tmp2)) {
		    /* version 0 */
		    expires = mymktime(tmp2->ptr);
		}
		else if (matchattr(p, "max-age", 7, &tmp2)) {
		    /* XXX Is there any problem with max-age=0? (RFC 2109 ss. 4.2.1, 4.2.2 */
		    expires = time(NULL) + atol(tmp2->ptr);
		}
		else if (matchattr(p, "domain", 6, &tmp2)) {
		    domain = tmp2;
		}
		else if (matchattr(p, "path", 4, &tmp2)) {
		    path = tmp2;
		}
		else if (matchattr(p, "secure", 6, NULL)) {
		    flag |= COO_SECURE;
		}
		else if (matchattr(p, "comment", 7, &tmp2)) {
		    comment = tmp2;
		}
		else if (matchattr(p, "version", 7, &tmp2)) {
		    version = atoi(tmp2->ptr);
		}
		else if (matchattr(p, "port", 4, &tmp2)) {
		    /* version 1, Set-Cookie2 */
		    port = tmp2;
		}
		else if (matchattr(p, "commentURL", 10, &tmp2)) {
		    /* version 1, Set-Cookie2 */
		    commentURL = tmp2;
		}
		else if (matchattr(p, "discard", 7, NULL)) {
		    /* version 1, Set-Cookie2 */
		    flag |= COO_DISCARD;
		}
		quoted = 0;
		while (!IS_ENDL(*p) && (quoted || *p != ';')) {
		    if (*p == '"')
			quoted = (quoted) ? 0 : 1;
		    p++;
		}
	    }
	    if (pu && name->length > 0) {
		int err;
		if (show_cookie) {
		    if (flag & COO_SECURE)
		        disp_message_nsec("Received a secured cookie", FALSE, 1,
				      TRUE, FALSE);
		    else
		        disp_message_nsec(Sprintf("Received cookie: %s=%s",
					      name->ptr, value->ptr)->ptr,
				      FALSE, 1, TRUE, FALSE);
		}
		err =
		    add_cookie(pu, name, value, expires, domain, path, flag,
			       comment, version, port, commentURL);
		if (err) {
		    char *ans = (accept_bad_cookie == ACCEPT_BAD_COOKIE_ACCEPT)
			? "y" : NULL;
		    if (fmInitialized && (err & COO_OVERRIDE_OK) &&
			accept_bad_cookie == ACCEPT_BAD_COOKIE_ASK) {
			Str msg = Sprintf("Accept bad cookie from %s for %s?",
					  pu->host,
					  ((domain && domain->ptr)
					   ? domain->ptr : "<localdomain>"));
			if (msg->length > COLS - 10)
			    Strshrink(msg, msg->length - (COLS - 10));
			Strcat_charp(msg, " (y/n)");
			ans = inputAnswer(msg->ptr);
		    }
		    if (ans == NULL || TOLOWER(*ans) != 'y' ||
			(err =
			 add_cookie(pu, name, value, expires, domain, path,
				    flag | COO_OVERRIDE, comment, version,
				    port, commentURL))) {
			err = (err & ~COO_OVERRIDE_OK) - 1;
			if (err >= 0 && err < COO_EMAX)
			    emsg = Sprintf("This cookie was rejected "
					   "to prevent security violation. [%s]",
					   violations[err])->ptr;
			else
			    emsg =
				"This cookie was rejected to prevent security violation.";
			record_err_message(emsg);
			if (show_cookie)
			    disp_message_nsec(emsg, FALSE, 1, TRUE, FALSE);
		    }
		    else
			if (show_cookie)
			    disp_message_nsec(Sprintf
					  ("Accepting invalid cookie: %s=%s",
					   name->ptr, value->ptr)->ptr, FALSE,
					  1, TRUE, FALSE);
		}
	    }
	}
#endif				/* USE_COOKIE */
	else if (!strncasecmp(lineBuf2->ptr, "w3m-control:", 12) &&
		 uf->scheme == SCM_LOCAL_CGI) {
	    Str funcname = Strnew();
	    int f;

	    p = lineBuf2->ptr + 12;
	    SKIP_BLANKS(p);
	    while (*p && !IS_SPACE(*p))
		Strcat_char(funcname, *(p++));
	    SKIP_BLANKS(p);
	    f = getFuncList(funcname->ptr);
	    if (f >= 0) {
		tmp = Strnew_charp(p);
		Strchop(tmp);
		pushEvent(f, tmp->ptr);
	    }
	}
	if (headerlist)
	    pushText(headerlist, lineBuf2->ptr);
	Strfree(lineBuf2);
	lineBuf2 = NULL;
    }
    if (thru)
	addnewline(newBuf, "", propBuffer, NULL, 0, -1, -1);
    if (src)
	fclose(src);
}