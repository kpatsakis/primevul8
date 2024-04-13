findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    int			flags,
    int			mincount)
{
    int		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = NULL;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (int)STRLEN(pat);
    st->orgpat->regmatch.regprog = NULL;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = NULL;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_count = 0;
    st->stop_searching = FALSE;

    for (mtt = 0; mtt < MT_COUNT; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], sizeof(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    if (st->tag_fname == NULL
	    || st->lbuf == NULL
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == NULL
#endif
       )
	return FAIL;

    return OK;
}