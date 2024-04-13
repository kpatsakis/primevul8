loadGeneralFile(char *path, ParsedURL *volatile current, char *referer,
		int flag, FormList *volatile request)
{
    URLFile f, *volatile of = NULL;
    ParsedURL pu;
    Buffer *b = NULL;
    Buffer *(*volatile proc)(URLFile *, Buffer *) = loadBuffer;
    char *volatile tpath;
    char *volatile t = "text/plain", *p, *volatile real_type = NULL;
    Buffer *volatile t_buf = NULL;
    int volatile searchHeader = SearchHeader;
    int volatile searchHeader_through = TRUE;
    MySignalHandler(*volatile prevtrap) (SIGNAL_ARG) = NULL;
    TextList *extra_header = newTextList();
    volatile Str uname = NULL;
    volatile Str pwd = NULL;
    volatile Str realm = NULL;
    int volatile add_auth_cookie_flag;
    unsigned char status = HTST_NORMAL;
    URLOption url_option;
    Str tmp;
    Str volatile page = NULL;
#ifdef USE_M17N
    wc_ces charset = WC_CES_US_ASCII;
#endif
    HRequest hr;
    ParsedURL *volatile auth_pu;

    tpath = path;
    prevtrap = NULL;
    add_auth_cookie_flag = 0;

    checkRedirection(NULL);

  load_doc:
    {
	const char *sc_redirect;
	parseURL2(tpath, &pu, current);
	sc_redirect = query_SCONF_SUBSTITUTE_URL(&pu);
	if (sc_redirect && *sc_redirect && checkRedirection(&pu)) {
	    tpath = (char *)sc_redirect;
	    request = NULL;
	    add_auth_cookie_flag = 0;
	    current = New(ParsedURL);
	    *current = pu;
	    status = HTST_NORMAL;
	    goto load_doc;
	}
    }
    TRAP_OFF;
    url_option.referer = referer;
    url_option.flag = flag;
    f = openURL(tpath, &pu, current, &url_option, request, extra_header, of,
		&hr, &status);
    of = NULL;
#ifdef USE_M17N
    content_charset = 0;
#endif
    if (f.stream == NULL) {
	switch (f.scheme) {
	case SCM_LOCAL:
	    {
		struct stat st;
		if (stat(pu.real_file, &st) < 0)
		    return NULL;
		if (S_ISDIR(st.st_mode)) {
		    if (UseExternalDirBuffer) {
			Str cmd = Sprintf("%s?dir=%s#current",
					  DirBufferCommand, pu.file);
			b = loadGeneralFile(cmd->ptr, NULL, NO_REFERER, 0,
					    NULL);
			if (b != NULL && b != NO_BUFFER) {
			    copyParsedURL(&b->currentURL, &pu);
			    b->filename = b->currentURL.real_file;
			}
			return b;
		    }
		    else {
			page = loadLocalDir(pu.real_file);
			t = "local:directory";
#ifdef USE_M17N
			charset = SystemCharset;
#endif
		    }
		}
	    }
	    break;
	case SCM_FTPDIR:
	    page = loadFTPDir(&pu, &charset);
	    t = "ftp:directory";
	    break;
#ifdef USE_NNTP
	case SCM_NEWS_GROUP:
	    page = loadNewsgroup(&pu, &charset);
	    t = "news:group";
	    break;
#endif
	case SCM_UNKNOWN:
#ifdef USE_EXTERNAL_URI_LOADER
	    tmp = searchURIMethods(&pu);
	    if (tmp != NULL) {
		b = loadGeneralFile(tmp->ptr, current, referer, flag, request);
		if (b != NULL && b != NO_BUFFER)
		    copyParsedURL(&b->currentURL, &pu);
		return b;
	    }
#endif
	    /* FIXME: gettextize? */
	    disp_err_message(Sprintf("Unknown URI: %s",
				     parsedURL2Str(&pu)->ptr)->ptr, FALSE);
	    break;
	}
	if (page && page->length > 0)
	    goto page_loaded;
	return NULL;
    }

    if (status == HTST_MISSING) {
	TRAP_OFF;
	UFclose(&f);
	return NULL;
    }

    /* openURL() succeeded */
    if (SETJMP(AbortLoading) != 0) {
	/* transfer interrupted */
	TRAP_OFF;
	if (b)
	    discardBuffer(b);
	UFclose(&f);
	return NULL;
    }

    b = NULL;
    if (f.is_cgi) {
	/* local CGI */
	searchHeader = TRUE;
	searchHeader_through = FALSE;
    }
    if (header_string)
	header_string = NULL;
    TRAP_ON;
    if (pu.scheme == SCM_HTTP ||
#ifdef USE_SSL
	pu.scheme == SCM_HTTPS ||
#endif				/* USE_SSL */
	((
#ifdef USE_GOPHER
	     (pu.scheme == SCM_GOPHER && non_null(GOPHER_proxy)) ||
#endif				/* USE_GOPHER */
	     (pu.scheme == SCM_FTP && non_null(FTP_proxy))
	 ) && !Do_not_use_proxy && !check_no_proxy(pu.host))) {

	if (fmInitialized) {
	    term_cbreak();
	    /* FIXME: gettextize? */
	    message(Sprintf("%s contacted. Waiting for reply...", pu.host)->
		    ptr, 0, 0);
	    refresh();
	}
	if (t_buf == NULL)
	    t_buf = newBuffer(INIT_BUFFER_WIDTH);
#if 0				/* USE_SSL */
	if (IStype(f.stream) == IST_SSL) {
	    Str s = ssl_get_certificate(f.stream, pu.host);
	    if (s == NULL)
		return NULL;
	    else
		t_buf->ssl_certificate = s->ptr;
	}
#endif
	readHeader(&f, t_buf, FALSE, &pu);
	if (((http_response_code >= 301 && http_response_code <= 303)
	     || http_response_code == 307)
	    && (p = checkHeader(t_buf, "Location:")) != NULL
	    && checkRedirection(&pu)) {
	    /* document moved */
	    /* 301: Moved Permanently */
	    /* 302: Found */
	    /* 303: See Other */
	    /* 307: Temporary Redirect (HTTP/1.1) */
	    tpath = url_encode(p, NULL, 0);
	    request = NULL;
	    UFclose(&f);
	    current = New(ParsedURL);
	    copyParsedURL(current, &pu);
	    t_buf = newBuffer(INIT_BUFFER_WIDTH);
	    t_buf->bufferprop |= BP_REDIRECTED;
	    status = HTST_NORMAL;
	    goto load_doc;
	}
	t = checkContentType(t_buf);
	if (t == NULL && pu.file != NULL) {
	    if (!((http_response_code >= 400 && http_response_code <= 407) ||
		  (http_response_code >= 500 && http_response_code <= 505)))
		t = guessContentType(pu.file);
	}
	if (t == NULL)
	    t = "text/plain";
	if (add_auth_cookie_flag && realm && uname && pwd) {
	    /* If authorization is required and passed */
	    add_auth_user_passwd(&pu, qstr_unquote(realm)->ptr, uname, pwd, 
				  0);
	    add_auth_cookie_flag = 0;
	}
	if ((p = checkHeader(t_buf, "WWW-Authenticate:")) != NULL &&
	    http_response_code == 401) {
	    /* Authentication needed */
	    struct http_auth hauth;
	    if (findAuthentication(&hauth, t_buf, "WWW-Authenticate:") != NULL
		&& (realm = get_auth_param(hauth.param, "realm")) != NULL) {
		auth_pu = &pu;
		getAuthCookie(&hauth, "Authorization:", extra_header,
			      auth_pu, &hr, request, &uname, &pwd);
		if (uname == NULL) {
		    /* abort */
		    TRAP_OFF;
		    goto page_loaded;
		}
		UFclose(&f);
		add_auth_cookie_flag = 1;
		status = HTST_NORMAL;
		goto load_doc;
	    }
	}
	if ((p = checkHeader(t_buf, "Proxy-Authenticate:")) != NULL &&
	    http_response_code == 407) {
	    /* Authentication needed */
	    struct http_auth hauth;
	    if (findAuthentication(&hauth, t_buf, "Proxy-Authenticate:")
		!= NULL
		&& (realm = get_auth_param(hauth.param, "realm")) != NULL) {
		auth_pu = schemeToProxy(pu.scheme);
		getAuthCookie(&hauth, "Proxy-Authorization:",
			      extra_header, auth_pu, &hr, request, 
			      &uname, &pwd);
		if (uname == NULL) {
		    /* abort */
		    TRAP_OFF;
		    goto page_loaded;
		}
		UFclose(&f);
		add_auth_cookie_flag = 1;
		status = HTST_NORMAL;
		add_auth_user_passwd(auth_pu, qstr_unquote(realm)->ptr, uname, pwd, 1);
		goto load_doc;
	    }
	}
	/* XXX: RFC2617 3.2.3 Authentication-Info: ? */

	if (status == HTST_CONNECT) {
	    of = &f;
	    goto load_doc;
	}

	f.modtime = mymktime(checkHeader(t_buf, "Last-Modified:"));
    }
#ifdef USE_NNTP
    else if (pu.scheme == SCM_NEWS || pu.scheme == SCM_NNTP) {
	if (t_buf == NULL)
	    t_buf = newBuffer(INIT_BUFFER_WIDTH);
	readHeader(&f, t_buf, TRUE, &pu);
	t = checkContentType(t_buf);
	if (t == NULL)
	    t = "text/plain";
    }
#endif				/* USE_NNTP */
#ifdef USE_GOPHER
    else if (pu.scheme == SCM_GOPHER) {
	switch (*pu.file) {
	case '0':
	    t = "text/plain";
	    break;
	case '1':
	case 'm':
	    page = loadGopherDir(&f, &pu, &charset);
	    t = "gopher:directory";
	    TRAP_OFF;
	    goto page_loaded;
	case 's':
	    t = "audio/basic";
	    break;
	case 'g':
	    t = "image/gif";
	    break;
	case 'h':
	    t = "text/html";
	    break;
	}
    }
#endif				/* USE_GOPHER */
    else if (pu.scheme == SCM_FTP) {
	check_compression(path, &f);
	if (f.compression != CMP_NOCOMPRESS) {
	    char *t1 = uncompressed_file_type(pu.file, NULL);
	    real_type = f.guess_type;
#if 0
	    if (t1 && strncasecmp(t1, "application/", 12) == 0) {
		f.compression = CMP_NOCOMPRESS;
		t = real_type;
	    }
	    else
#endif
	    if (t1)
		t = t1;
	    else
		t = real_type;
	}
	else {
	    real_type = guessContentType(pu.file);
	    if (real_type == NULL)
		real_type = "text/plain";
	    t = real_type;
	}
#if 0
	if (!strncasecmp(t, "application/", 12)) {
	    char *tmpf = tmpfname(TMPF_DFL, NULL)->ptr;
	    current_content_length = 0;
	    if (save2tmp(f, tmpf) < 0)
		UFclose(&f);
	    else {
		UFclose(&f);
		TRAP_OFF;
		doFileMove(tmpf, guess_save_name(t_buf, pu.file));
	    }
	    return NO_BUFFER;
	}
#endif
    }
    else if (pu.scheme == SCM_DATA) {
	t = f.guess_type;
    }
    else if (searchHeader) {
	searchHeader = SearchHeader = FALSE;
	if (t_buf == NULL)
	    t_buf = newBuffer(INIT_BUFFER_WIDTH);
	readHeader(&f, t_buf, searchHeader_through, &pu);
	if (f.is_cgi && (p = checkHeader(t_buf, "Location:")) != NULL &&
	    checkRedirection(&pu)) {
	    /* document moved */
	    tpath = url_encode(remove_space(p), NULL, 0);
	    request = NULL;
	    UFclose(&f);
	    add_auth_cookie_flag = 0;
	    current = New(ParsedURL);
	    copyParsedURL(current, &pu);
	    t_buf = newBuffer(INIT_BUFFER_WIDTH);
	    t_buf->bufferprop |= BP_REDIRECTED;
	    status = HTST_NORMAL;
	    goto load_doc;
	}
#ifdef AUTH_DEBUG
	if ((p = checkHeader(t_buf, "WWW-Authenticate:")) != NULL) {
	    /* Authentication needed */
	    struct http_auth hauth;
	    if (findAuthentication(&hauth, t_buf, "WWW-Authenticate:") != NULL
		&& (realm = get_auth_param(hauth.param, "realm")) != NULL) {
		auth_pu = &pu;
		getAuthCookie(&hauth, "Authorization:", extra_header,
			      auth_pu, &hr, request, &uname, &pwd);
		if (uname == NULL) {
		    /* abort */
		    TRAP_OFF;
		    goto page_loaded;
		}
		UFclose(&f);
		add_auth_cookie_flag = 1;
		status = HTST_NORMAL;
		goto load_doc;
	    }
	}
#endif /* defined(AUTH_DEBUG) */
	t = checkContentType(t_buf);
	if (t == NULL)
	    t = "text/plain";
    }
    else if (DefaultType) {
	t = DefaultType;
	DefaultType = NULL;
    }
    else {
	t = guessContentType(pu.file);
	if (t == NULL)
	    t = "text/plain";
	real_type = t;
	if (f.guess_type)
	    t = f.guess_type;
    }

    /* XXX: can we use guess_type to give the type to loadHTMLstream
     *      to support default utf8 encoding for XHTML here? */
    f.guess_type = t;
    
  page_loaded:
    if (page) {
	FILE *src;
#ifdef USE_IMAGE
	if (image_source)
	    return NULL;
#endif
	tmp = tmpfname(TMPF_SRC, ".html");
	src = fopen(tmp->ptr, "w");
	if (src) {
	    Str s;
	    s = wc_Str_conv_strict(page, InnerCharset, charset);
	    Strfputs(s, src);
	    fclose(src);
	}
	if (do_download) {
	    char *file;
	    if (!src)
		return NULL;
	    file = guess_filename(pu.file);
#ifdef USE_GOPHER
	    if (f.scheme == SCM_GOPHER)
		file = Sprintf("%s.html", file)->ptr;
#endif
#ifdef USE_NNTP
	    if (f.scheme == SCM_NEWS_GROUP)
		file = Sprintf("%s.html", file)->ptr;
#endif
	    doFileMove(tmp->ptr, file);
	    return NO_BUFFER;
	}
	b = loadHTMLString(page);
	if (b) {
	    copyParsedURL(&b->currentURL, &pu);
	    b->real_scheme = pu.scheme;
	    b->real_type = t;
	    if (src)
		b->sourcefile = tmp->ptr;
#ifdef USE_M17N
	    b->document_charset = charset;
#endif
	}
	return b;
    }

    if (real_type == NULL)
	real_type = t;
    proc = loadBuffer;

    current_content_length = 0;
    if ((p = checkHeader(t_buf, "Content-Length:")) != NULL)
	current_content_length = strtoclen(p);
    if (do_download) {
	/* download only */
	char *file;
	TRAP_OFF;
	if (DecodeCTE && IStype(f.stream) != IST_ENCODED)
	    f.stream = newEncodedStream(f.stream, f.encoding);
	if (pu.scheme == SCM_LOCAL) {
	    struct stat st;
	    if (PreserveTimestamp && !stat(pu.real_file, &st))
		f.modtime = st.st_mtime;
	    file = conv_from_system(guess_save_name(NULL, pu.real_file));
	}
	else
	    file = guess_save_name(t_buf, pu.file);
	if (doFileSave(f, file) == 0)
	    UFhalfclose(&f);
	else
	    UFclose(&f);
	return NO_BUFFER;
    }

    if ((f.content_encoding != CMP_NOCOMPRESS) && AutoUncompress
	&& !(w3m_dump & DUMP_EXTRA)) {
	uncompress_stream(&f, &pu.real_file);
    }
    else if (f.compression != CMP_NOCOMPRESS) {
	if (!(w3m_dump & DUMP_SOURCE) &&
	    (w3m_dump & ~DUMP_FRAME || is_text_type(t)
	     || searchExtViewer(t))) {
	    if (t_buf == NULL)
		t_buf = newBuffer(INIT_BUFFER_WIDTH);
	    uncompress_stream(&f, &t_buf->sourcefile);
	    uncompressed_file_type(pu.file, &f.ext);
	}
	else {
	    t = compress_application_type(f.compression);
	    f.compression = CMP_NOCOMPRESS;
	}
    }
#ifdef USE_IMAGE
    if (image_source) {
	Buffer *b = NULL;
	if (IStype(f.stream) != IST_ENCODED)
	    f.stream = newEncodedStream(f.stream, f.encoding);
	if (save2tmp(f, image_source) == 0) {
	    b = newBuffer(INIT_BUFFER_WIDTH);
	    b->sourcefile = image_source;
	    b->real_type = t;
	}
	UFclose(&f);
	TRAP_OFF;
	return b;
    }
#endif

    if (is_html_type(t))
	proc = loadHTMLBuffer;
    else if (is_plain_text_type(t))
	proc = loadBuffer;
#ifdef USE_IMAGE
    else if (activeImage && displayImage && !useExtImageViewer &&
	     !(w3m_dump & ~DUMP_FRAME) && !strncasecmp(t, "image/", 6))
	proc = loadImageBuffer;
#endif
    else if (w3m_backend) ;
    else if (!(w3m_dump & ~DUMP_FRAME) || is_dump_text_type(t)) {
	if (!do_download && searchExtViewer(t) != NULL) {
	    proc = DO_EXTERNAL;
	}
	else {
	    TRAP_OFF;
	    if (pu.scheme == SCM_LOCAL) {
		UFclose(&f);
		_doFileCopy(pu.real_file,
			    conv_from_system(guess_save_name
					     (NULL, pu.real_file)), TRUE);
	    }
	    else {
		if (DecodeCTE && IStype(f.stream) != IST_ENCODED)
		    f.stream = newEncodedStream(f.stream, f.encoding);
		if (doFileSave(f, guess_save_name(t_buf, pu.file)) == 0)
		    UFhalfclose(&f);
		else
		    UFclose(&f);
	    }
	    return NO_BUFFER;
	}
    }
    else if (w3m_dump & DUMP_FRAME)
	return NULL;

    if (t_buf == NULL)
	t_buf = newBuffer(INIT_BUFFER_WIDTH);
    copyParsedURL(&t_buf->currentURL, &pu);
    t_buf->filename = pu.real_file ? pu.real_file :
	pu.file ? conv_to_system(pu.file) : NULL;
    if (flag & RG_FRAME) {
	t_buf->bufferprop |= BP_FRAME;
    }
#ifdef USE_SSL
    t_buf->ssl_certificate = f.ssl_certificate;
#endif
    frame_source = flag & RG_FRAME_SRC;
    if (proc == DO_EXTERNAL) {
	b = doExternal(f, t, t_buf);
    } else {
	b = loadSomething(&f, proc, t_buf);
    }
    UFclose(&f);
    frame_source = 0;
    if (b && b != NO_BUFFER) {
	b->real_scheme = f.scheme;
	b->real_type = real_type;
	if (w3m_backend)
	    b->type = allocStr(t, -1);
	if (pu.label) {
	    if (proc == loadHTMLBuffer) {
		Anchor *a;
		a = searchURLLabel(b, pu.label);
		if (a != NULL) {
		    gotoLine(b, a->start.line);
		    if (label_topline)
			b->topLine = lineSkip(b, b->topLine,
					      b->currentLine->linenumber
					      - b->topLine->linenumber, FALSE);
		    b->pos = a->start.pos;
		    arrangeCursor(b);
		}
	    }
	    else {		/* plain text */
		int l = atoi(pu.label);
		gotoRealLine(b, l);
		b->pos = 0;
		arrangeCursor(b);
	    }
	}
    }
    if (header_string)
	header_string = NULL;
#ifdef USE_NNTP
    if (b && b != NO_BUFFER && (f.scheme == SCM_NNTP || f.scheme == SCM_NEWS))
	reAnchorNewsheader(b);
#endif
    if (b && b != NO_BUFFER)
	preFormUpdateBuffer(b);
    TRAP_OFF;
    return b;
}