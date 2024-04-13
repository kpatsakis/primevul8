findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	search_info;
    int			retval;
#ifdef FEAT_CSCOPE
    int			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(search_info);

    // Read and parse the lines in the file one by one
    for (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	if (st->state == TS_BINARY || st->state == TS_SKIP_BACK)
	    line_breakcheck();
	else
	    fast_breakcheck();
	if ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_keys(30, FALSE);
	if (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    break;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	if (st->mincount == TAG_MANY && st->match_count >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    break;
	}
	if (st->get_searchpat)
	    goto line_read_in;

	retval = findtags_get_next_line(st, &search_info);
	if (retval == TAGS_READ_IGNORE)
	    continue;
	if (retval == TAGS_READ_EOF)
	    break;

line_read_in:

	if (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	if (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    continue;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	if (st->state == TS_START)
	{
	    if (findtags_start_state_handler(st, &margs->sortic, &search_info) == FALSE)
		continue;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	if (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    if (st->lbuf == NULL)
	    {
		if (st->fp != NULL)
		    fclose(st->fp);
		st->fp = NULL;
		st->stop_searching = TRUE;
		return;
	    }

	    if (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, search_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    search_info.curr_offset = 0;
	    continue;
	}

	retval = findtags_parse_line(st, &tagp, margs, &search_info);
	if (retval == TAG_MATCH_NEXT)
	    continue;
	if (retval == TAG_MATCH_STOP)
	    break;
	if (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    if (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    return;
	}

	retval = findtags_match_tag(st, &tagp, margs);
	if (retval == TAG_MATCH_NEXT)
	    continue;
	if (retval == TAG_MATCH_STOP)
	    break;

	// If a match is found, add it to ht_match[] and ga_match[].
	if (retval == TAG_MATCH_SUCCESS)
	{
	    if (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		break;
	}
    } // forever
}