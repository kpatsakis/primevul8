findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    int		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    int		retval;

    if (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	return NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_count,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    return retval;
}