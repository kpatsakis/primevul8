prepare_pats(pat_T *pats, int has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    if (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	if (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	else if (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	if (pats->head == pats->pat)
	    pats->headlen = 0;
	else
	    for (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		if (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != NULL)
		    break;
	if (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    if (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    else
	pats->regmatch.regprog = NULL;
}