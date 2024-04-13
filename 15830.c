findtags_in_help_init(findtags_state_T *st)
{
    int		i;
    char_u	*s;

    // Keep 'en' as the language if the file extension is '.txt'
    if (st->is_txt)
	STRCPY(st->help_lang, "en");
    else
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (int)STRLEN(st->tag_fname);
	if (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	else
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    if (st->help_lang_find != NULL
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	return FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    if ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == NULL
	    && curbuf->b_fname != NULL
	    && (i = (int)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    else
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	for (s = p_hlg; *s != NUL; ++s)
	{
	    if (STRNICMP(s, st->help_lang, 2) == 0)
		break;
	    ++st->help_pri;
	    if ((s = vim_strchr(s, ',')) == NULL)
		break;
	}
	if (s == NULL || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    if (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    return TRUE;
}