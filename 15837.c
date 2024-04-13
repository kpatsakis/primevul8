findtags_parse_line(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    int		status;
    int		i;
    int		cmplen;
    int		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    if (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	if (tagpp->tagname_end == NULL)
	    // Corrupted tag line.
	    return TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (int)(tagpp->tagname_end - tagpp->tagname);
	if (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	if ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	else if (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    return TAG_MATCH_NEXT;

	if (st->state == TS_BINARY)
	{
	    // Simplistic check for unsorted tags file.
	    i = (int)tagpp->tagname[0];
	    if (margs->sortic)
		i = (int)TOUPPER_ASC(tagpp->tagname[0]);
	    if (i < sinfo_p->low_char || i > sinfo_p->high_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    if (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    else
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    if (tagcmp == 0)
	    {
		if (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		else if (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    if (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		return TAG_MATCH_NEXT;
	    }
	    if (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		if (sinfo_p->curr_offset < sinfo_p->high_offset)
		{
		    sinfo_p->low_offset = sinfo_p->curr_offset;
		    if (margs->sortic)
			sinfo_p->low_char = TOUPPER_ASC(tagpp->tagname[0]);
		    else
			sinfo_p->low_char = tagpp->tagname[0];
		    return TAG_MATCH_NEXT;
		}
	    }
	    if (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->high_offset)
	    {
		sinfo_p->high_offset = sinfo_p->curr_offset;
		if (margs->sortic)
		    sinfo_p->high_char = TOUPPER_ASC(tagpp->tagname[0]);
		else
		    sinfo_p->high_char = tagpp->tagname[0];
		return TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    return TAG_MATCH_STOP;
	}
	else if (st->state == TS_SKIP_BACK)
	{
	    if (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    else
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    return TAG_MATCH_NEXT;
	}
	else if (st->state == TS_STEP_FORWARD)
	{
	    if (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		if ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    return TAG_MATCH_STOP;	// past last match
		else
		    return TAG_MATCH_NEXT;	// before first match
	    }
	}
	else
	    // skip this match if it can't match
	    if (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		return TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	if (tagpp->fname_end == NULL)
	    status = FAIL;
	else
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    else
	status = parse_tag_line(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    if (status == FAIL)
	return TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    if (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    return TAG_MATCH_SUCCESS;
}