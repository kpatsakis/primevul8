add_pack_dir_to_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *entry;
    char_u  *insp = NULL;
    int	    c;
    char_u  *new_rtp;
    int	    keep;
    size_t  oldlen;
    size_t  addlen;
    size_t  new_rtp_len;
    char_u  *afterdir = NULL;
    size_t  afterlen = 0;
    char_u  *after_insp = NULL;
    char_u  *ffname = NULL;
    size_t  fname_len;
    char_u  *buf = NULL;
    char_u  *rtp_ffname;
    int	    match;
    int	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_head(fname);
    for (p = p1; *p; MB_PTR_ADV(p))
	if (vim_ispathsep_nocolon(*p))
	{
	    p4 = p3; p3 = p2; p2 = p1; p1 = p;
	}

    // now we have:
    // rtp/pack/name/start/name
    //    p4   p3   p2    p1
    //
    // find the part up to "pack" in 'runtimepath'
    c = *++p4; // append pathsep in order to expand symlink
    *p4 = NUL;
    ffname = fix_fname(fname);
    *p4 = c;
    if (ffname == NULL)
	return FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf = alloc(MAXPATHL);
    if (buf == NULL)
	goto theend;
    for (entry = p_rtp; *entry != NUL; )
    {
	char_u *cur_entry = entry;

	copy_option_part(&entry, buf, MAXPATHL, ",");
	if (insp == NULL)
	{
	    add_pathsep(buf);
	    rtp_ffname = fix_fname(buf);
	    if (rtp_ffname == NULL)
		goto theend;
	    match = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    if (match)
		// Insert "ffname" after this entry (and comma).
		insp = entry;
	}

	if ((p = (char_u *)strstr((char *)buf, "after")) != NULL
		&& p > buf
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    if (insp == NULL)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_entry;
	    after_insp = cur_entry;
	    break;
	}
    }

    if (insp == NULL)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + STRLEN(p_rtp);

    // check if rtp/pack/name/start/name/after exists
    afterdir = concat_fnames(fname, (char_u *)"after", TRUE);
    if (afterdir != NULL && mch_isdir(afterdir))
	afterlen = STRLEN(afterdir) + 1; // add one for comma

    oldlen = STRLEN(p_rtp);
    addlen = STRLEN(fname) + 1; // add one for comma
    new_rtp = alloc(oldlen + addlen + afterlen + 1); // add one for NUL
    if (new_rtp == NULL)
	goto theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (int)(insp - p_rtp);
    mch_memmove(new_rtp, p_rtp, keep);
    new_rtp_len = keep;
    if (*insp == NUL)
	new_rtp[new_rtp_len++] = ',';  // add comma before
    mch_memmove(new_rtp + new_rtp_len, fname, addlen - 1);
    new_rtp_len += addlen - 1;
    if (*insp != NUL)
	new_rtp[new_rtp_len++] = ',';  // add comma after

    if (afterlen > 0 && after_insp != NULL)
    {
	int keep_after = (int)(after_insp - p_rtp);

	// Add to new_rtp: {keep},{fname}{keep_after},{afterdir}
	mch_memmove(new_rtp + new_rtp_len, p_rtp + keep,
							keep_after - keep);
	new_rtp_len += keep_after - keep;
	mch_memmove(new_rtp + new_rtp_len, afterdir, afterlen - 1);
	new_rtp_len += afterlen - 1;
	new_rtp[new_rtp_len++] = ',';
	keep = keep_after;
    }

    if (p_rtp[keep] != NUL)
	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp + new_rtp_len, p_rtp + keep, oldlen - keep + 1);
    else
	new_rtp[new_rtp_len] = NUL;

    if (afterlen > 0 && after_insp == NULL)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	STRCAT(new_rtp, ",");
	STRCAT(new_rtp, afterdir);
    }

    set_option_value((char_u *)"rtp", 0L, new_rtp, 0);
    vim_free(new_rtp);
    retval = OK;

theend:
    vim_free(buf);
    vim_free(ffname);
    vim_free(afterdir);
    return retval;
}