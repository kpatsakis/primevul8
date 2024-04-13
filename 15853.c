test_for_current(
#ifdef FEAT_EMACS_TAGS
    int	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    int	    c;
    int	    retval = FALSE;
    char_u  *fullname;

    if (buf_ffname != NULL)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	if (is_etag)
	    c = 0;	    // to shut up GCC
	else
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	if (fullname != NULL)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	if (!is_etag)
#endif
	    *fname_end = c;
    }

    return retval;
}