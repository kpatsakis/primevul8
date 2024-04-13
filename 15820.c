parse_tag_line(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    int		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    if (is_etag)
	// emacs-style tag file
	return emacs_tags_parse_line(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    if (p == NULL)
	return FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    if (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    if (p == NULL)
	return FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    if (*p != NUL)
	++p;
    if (*p == NUL)
	return FAIL;
    tagp->command = p;

    return OK;
}