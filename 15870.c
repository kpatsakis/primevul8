findtags_get_next_line(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    int		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    if (st->state == TS_BINARY)
    {
	offset = sinfo_p->low_offset + ((sinfo_p->high_offset
						- sinfo_p->low_offset) / 2);
	if (offset == sinfo_p->curr_offset)
	    return TAGS_READ_EOF; // End the binary search without a match.
	else
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    else if (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	if (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    if (st->state == TS_BINARY || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	if (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    if (sinfo_p->curr_offset == sinfo_p->high_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->low_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->low_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	while (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	if (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    return TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    else
    {
	// skip empty and blank lines
	do
	{
#ifdef FEAT_CSCOPE
	    if (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    else
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} while (!eof && vim_isblankline(st->lbuf));

	if (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    if (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		return TAGS_READ_IGNORE;
#endif
	    return TAGS_READ_EOF;
	}
    }

    return TAGS_READ_SUCCESS;
}