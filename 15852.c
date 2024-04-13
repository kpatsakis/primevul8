findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    int		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    int		mtt;
    int		i;
    char_u	*mfp;
    char_u	*p;

    if (st->match_count > 0)
	matches = ALLOC_MULT(char_u *, st->match_count);
    else
	matches = NULL;
    st->match_count = 0;
    for (mtt = 0; mtt < MT_COUNT; ++mtt)
    {
	for (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    if (matches == NULL)
		vim_free(mfp);
	    else
	    {
		if (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    for (p = mfp + 1; *p != NUL; ++p)
			if (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_count++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    return st->match_count;
}