findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    int		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    int		name_only = (st->flags & TAG_NAMES);
    int		mtt;
    int		len = 0;
    int		is_current;		// file name matches
    int		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    if (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    else
#endif
    {
	// Decide in which array to store this match.
	is_current = test_for_current(
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp->fname, tagpp->fname_end, st->tag_fname, buf_ffname);
#ifdef FEAT_EMACS_TAGS
	is_static = FALSE;
	if (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	if (is_static)
	{
	    if (is_current)
		mtt = MT_ST_CUR;
	    else
		mtt = MT_ST_OTH;
	}
	else
	{
	    if (is_current)
		mtt = MT_GL_CUR;
	    else
		mtt = MT_GL_OTH;
	}
	if (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	if (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    if (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#else
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (int)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(sizeof(char_u) + len + 10 + ML_EXTRA + 1);
	if (mfp != NULL)
	{
	    int heuristic;

	    p = mfp;
	    STRCPY(p, tagpp->tagname);
#ifdef FEAT_MULTI_LANG
	    p[len] = '@';
	    STRCPY(p + len + 1, st->help_lang);
#endif

	    heuristic = help_heuristic(tagpp->tagname,
				margs->match_re ? margs->matchoff : 0,
				!margs->match_no_ic);
#ifdef FEAT_MULTI_LANG
	    heuristic += st->help_pri;
#endif
	    sprintf((char *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    else if (name_only)
    {
	if (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    if (*temp_end == '/')
		while (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    if (tagpp->command + 2 < temp_end)
	    {
		len = (int)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		if (mfp != NULL)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    else
		mfp = NULL;
	    st->get_searchpat = FALSE;
	}
	else
	{
	    len = (int)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(sizeof(char_u) + len + 1);
	    if (mfp != NULL)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    if (State & MODE_INSERT)
		st->get_searchpat = p_sft;
	}
    }
    else
    {
	size_t tag_fname_len = STRLEN(st->tag_fname);
#ifdef FEAT_EMACS_TAGS
	size_t ebuf_len = 0;
#endif

	// Save the tag in a buffer.
	// Use 0x02 to separate fields (Can't use NUL because the
	// hash key is terminated by NUL, or Ctrl_A because that is
	// part of some Emacs tag files -- see parse_tag_line).
	// Emacs tag: <mtt><tag_fname><0x02><ebuf><0x02><lbuf><NUL>
	// other tag: <mtt><tag_fname><0x02><0x02><lbuf><NUL>
	// without Emacs tags: <mtt><tag_fname><0x02><lbuf><NUL>
	// Here <mtt> is the "mtt" value plus 1 to avoid NUL.
	len = (int)tag_fname_len + (int)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	if (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (int)ebuf_len + 1;
	}
	else
	    ++len;
#endif
	mfp = alloc(sizeof(char_u) + len + 1);
	if (mfp != NULL)
	{
	    p = mfp;
	    p[0] = mtt + 1;
	    STRCPY(p + 1, st->tag_fname);
#ifdef BACKSLASH_IN_FILENAME
	    // Ignore differences in slashes, avoid adding
	    // both path/file and path\file.
	    slash_adjust(p + 1);
#endif
	    p[tag_fname_len + 1] = TAG_SEP;
	    s = p + 1 + tag_fname_len + 1;
#ifdef FEAT_EMACS_TAGS
	    if (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    else
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    if (mfp != NULL)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	if (use_cscope)
	    ++*hash;
	else
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	if (HASHITEM_EMPTY(hi))
	{
	    if (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) != OK)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		return FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_count++;
	}
	else
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    return OK;
}