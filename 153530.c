process_img(struct parsed_tag *tag, int width)
{
    char *p, *q, *r, *r2 = NULL, *s, *t;
#ifdef USE_IMAGE
    int w, i, nw, ni = 1, n, w0 = -1, i0 = -1;
    int align, xoffset, yoffset, top, bottom, ismap = 0;
    int use_image = activeImage && displayImage;
#else
    int w, i, nw, n;
#endif
    int pre_int = FALSE, ext_pre_int = FALSE;
    Str tmp = Strnew();

    if (!parsedtag_get_value(tag, ATTR_SRC, &p))
	return tmp;
    p = url_encode(remove_space(p), cur_baseURL, cur_document_charset);
    q = NULL;
    parsedtag_get_value(tag, ATTR_ALT, &q);
    if (!pseudoInlines && (q == NULL || (*q == '\0' && ignore_null_img_alt)))
	return tmp;
    t = q;
    parsedtag_get_value(tag, ATTR_TITLE, &t);
    w = -1;
    if (parsedtag_get_value(tag, ATTR_WIDTH, &w)) {
	if (w < 0) {
	    if (width > 0)
		w = (int)(-width * pixel_per_char * w / 100 + 0.5);
	    else
		w = -1;
	}
#ifdef USE_IMAGE
	if (use_image) {
	    if (w > 0) {
		w = (int)(w * image_scale / 100 + 0.5);
		if (w == 0)
		    w = 1;
		else if (w > MAX_IMAGE_SIZE)
		    w = MAX_IMAGE_SIZE;
	    }
	}
#endif
    }
#ifdef USE_IMAGE
    if (use_image) {
	i = -1;
	if (parsedtag_get_value(tag, ATTR_HEIGHT, &i)) {
	    if (i > 0) {
		i = (int)(i * image_scale / 100 + 0.5);
		if (i == 0)
		    i = 1;
		else if (i > MAX_IMAGE_SIZE)
		    i = MAX_IMAGE_SIZE;
	    }
	    else {
		i = -1;
	    }
	}
	align = -1;
	parsedtag_get_value(tag, ATTR_ALIGN, &align);
	ismap = 0;
	if (parsedtag_exists(tag, ATTR_ISMAP))
	    ismap = 1;
    }
    else
#endif
	parsedtag_get_value(tag, ATTR_HEIGHT, &i);
    r = NULL;
    parsedtag_get_value(tag, ATTR_USEMAP, &r);
    if (parsedtag_exists(tag, ATTR_PRE_INT))
	ext_pre_int = TRUE;

    tmp = Strnew_size(128);
#ifdef USE_IMAGE
    if (use_image) {
	switch (align) {
	case ALIGN_LEFT:
	    Strcat_charp(tmp, "<div_int align=left>");
	    break;
	case ALIGN_CENTER:
	    Strcat_charp(tmp, "<div_int align=center>");
	    break;
	case ALIGN_RIGHT:
	    Strcat_charp(tmp, "<div_int align=right>");
	    break;
	}
    }
#endif
    if (r) {
	Str tmp2;
	r2 = strchr(r, '#');
	s = "<form_int method=internal action=map>";
	tmp2 = process_form(parse_tag(&s, TRUE));
	if (tmp2)
	    Strcat(tmp, tmp2);
	Strcat(tmp, Sprintf("<input_alt fid=\"%d\" "
			    "type=hidden name=link value=\"", cur_form_id));
	Strcat_charp(tmp, html_quote((r2) ? r2 + 1 : r));
	Strcat(tmp, Sprintf("\"><input_alt hseq=\"%d\" fid=\"%d\" "
			    "type=submit no_effect=true>",
			    cur_hseq++, cur_form_id));
    }
#ifdef USE_IMAGE
    if (use_image) {
	w0 = w;
	i0 = i;
	if (w < 0 || i < 0) {
	    Image image;
	    ParsedURL u;

	    parseURL2(p, &u, cur_baseURL);
	    image.url = parsedURL2Str(&u)->ptr;
	    if (!uncompressed_file_type(u.file, &image.ext))
		image.ext = filename_extension(u.file, TRUE);
	    image.cache = NULL;
	    image.width = w;
	    image.height = i;

	    image.cache = getImage(&image, cur_baseURL, IMG_FLAG_SKIP);
	    if (image.cache && image.cache->width > 0 &&
		image.cache->height > 0) {
		w = w0 = image.cache->width;
		i = i0 = image.cache->height;
	    }
	    if (w < 0)
		w = 8 * pixel_per_char;
	    if (i < 0)
		i = pixel_per_line;
	}
	if (enable_inline_image) {
	    nw = (w > 1) ? ((w - 1) / pixel_per_char_i + 1) : 1 ;
	    ni = (i > 1) ? ((i - 1) / pixel_per_line_i + 1) : 1 ;
	}
	else {
	    nw = (w > 3) ? (int)((w - 3) / pixel_per_char + 1) : 1;
	    ni = (i > 3) ? (int)((i - 3) / pixel_per_line + 1) : 1;
	}
	Strcat(tmp,
	       Sprintf("<pre_int><img_alt hseq=\"%d\" src=\"", cur_iseq++));
	pre_int = TRUE;
    }
    else
#endif
    {
	if (w < 0)
	    w = 12 * pixel_per_char;
	nw = w ? (int)((w - 1) / pixel_per_char + 1) : 1;
	if (r) {
	    Strcat_charp(tmp, "<pre_int>");
	    pre_int = TRUE;
	}
	Strcat_charp(tmp, "<img_alt src=\"");
    }
    Strcat_charp(tmp, html_quote(p));
    Strcat_charp(tmp, "\"");
    if (t) {
	Strcat_charp(tmp, " title=\"");
	Strcat_charp(tmp, html_quote(t));
	Strcat_charp(tmp, "\"");
    }
#ifdef USE_IMAGE
    if (use_image) {
	if (w0 >= 0)
	    Strcat(tmp, Sprintf(" width=%d", w0));
	if (i0 >= 0)
	    Strcat(tmp, Sprintf(" height=%d", i0));
	switch (align) {
	case ALIGN_MIDDLE:
	    if (!enable_inline_image) {
		top = ni / 2;
		bottom = top;
		if (top * 2 == ni)
		    yoffset = (int)(((ni + 1) * pixel_per_line - i) / 2);
		else
		    yoffset = (int)((ni * pixel_per_line - i) / 2);
		break;
	    }
	case ALIGN_TOP:
	    top = 0;
	    bottom = ni - 1;
	    yoffset = 0;
	    break;
	case ALIGN_BOTTOM:
	    top = ni - 1;
	    bottom = 0;
	    yoffset = (int)(ni * pixel_per_line - i);
	    break;
	default:
	    top = ni - 1;
	    bottom = 0;
	    if (ni == 1 && ni * pixel_per_line > i)
		yoffset = 0;
	    else {
		yoffset = (int)(ni * pixel_per_line - i);
		if (yoffset <= -2)
		    yoffset++;
	    }
	    break;
	}

	if (enable_inline_image)
	    xoffset = 0;
	else
	    xoffset = (int)((nw * pixel_per_char - w) / 2);

	if (xoffset)
	    Strcat(tmp, Sprintf(" xoffset=%d", xoffset));
	if (yoffset)
	    Strcat(tmp, Sprintf(" yoffset=%d", yoffset));
	if (top)
	    Strcat(tmp, Sprintf(" top_margin=%d", top));
	if (bottom)
	    Strcat(tmp, Sprintf(" bottom_margin=%d", bottom));
	if (r) {
	    Strcat_charp(tmp, " usemap=\"");
	    Strcat_charp(tmp, html_quote((r2) ? r2 + 1 : r));
	    Strcat_charp(tmp, "\"");
	}
	if (ismap)
	    Strcat_charp(tmp, " ismap");
    }
#endif
    Strcat_charp(tmp, ">");
    if (q != NULL && *q == '\0' && ignore_null_img_alt)
	q = NULL;
    if (q != NULL) {
	n = get_strwidth(q);
#ifdef USE_IMAGE
	if (use_image) {
	    if (n > nw) {
		char *r;
		for (r = q, n = 0; r; r += get_mclen(r), n += get_mcwidth(r)) {
		    if (n + get_mcwidth(r) > nw)
			break;
		}
		Strcat_charp(tmp, html_quote(Strnew_charp_n(q, r - q)->ptr));
	    }
	    else
		Strcat_charp(tmp, html_quote(q));
	}
	else
#endif
	    Strcat_charp(tmp, html_quote(q));
	goto img_end;
    }
    if (w > 0 && i > 0) {
	/* guess what the image is! */
	if (w < 32 && i < 48) {
	    /* must be an icon or space */
	    n = 1;
	    if (strcasestr(p, "space") || strcasestr(p, "blank"))
		Strcat_charp(tmp, "_");
	    else {
		if (w * i < 8 * 16)
		    Strcat_charp(tmp, "*");
		else {
		    if (!pre_int) {
			Strcat_charp(tmp, "<pre_int>");
			pre_int = TRUE;
		    }
		    push_symbol(tmp, IMG_SYMBOL, symbol_width, 1);
		    n = symbol_width;
		}
	    }
	    goto img_end;
	}
	if (w > 200 && i < 13) {
	    /* must be a horizontal line */
	    if (!pre_int) {
		Strcat_charp(tmp, "<pre_int>");
		pre_int = TRUE;
	    }
	    w = w / pixel_per_char / symbol_width;
	    if (w <= 0)
		w = 1;
	    push_symbol(tmp, HR_SYMBOL, symbol_width, w);
	    n = w * symbol_width;
	    goto img_end;
	}
    }
    for (q = p; *q; q++) ;
    while (q > p && *q != '/')
	q--;
    if (*q == '/')
	q++;
    Strcat_char(tmp, '[');
    n = 1;
    p = q;
    for (; *q; q++) {
	if (!IS_ALNUM(*q) && *q != '_' && *q != '-') {
	    break;
	}
	Strcat_char(tmp, *q);
	n++;
	if (n + 1 >= nw)
	    break;
    }
    Strcat_char(tmp, ']');
    n++;
  img_end:
#ifdef USE_IMAGE
    if (use_image) {
	for (; n < nw; n++)
	    Strcat_char(tmp, ' ');
    }
#endif
    Strcat_charp(tmp, "</img_alt>");
    if (pre_int && !ext_pre_int)
	Strcat_charp(tmp, "</pre_int>");
    if (r) {
	Strcat_charp(tmp, "</input_alt>");
	process_n_form();
    }
#ifdef USE_IMAGE
    if (use_image) {
	switch (align) {
	case ALIGN_RIGHT:
	case ALIGN_CENTER:
	case ALIGN_LEFT:
	    Strcat_charp(tmp, "</div_int>");
	    break;
	}
    }
#endif
    return tmp;
}