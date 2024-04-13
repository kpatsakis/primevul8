get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    int		num_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &num_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (int)MAXCOL, buf_fname);
    if (ret == OK && num_matches > 0)
    {
	for (i = 0; i < num_matches; ++i)
	{
	    parse_match(matches[i], &tp);
	    is_static = test_for_static(&tp);

	    // Skip pseudo-tag lines.
	    if (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	    {
		vim_free(matches[i]);
		continue;
	    }

	    if ((dict = dict_alloc()) == NULL)
		ret = FAIL;
	    if (list_append_dict(list, dict) == FAIL)
		ret = FAIL;

	    full_fname = tag_full_fname(&tp);
	    if (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		    || add_tag_field(dict, "filename", full_fname,
							 NULL) == FAIL
		    || add_tag_field(dict, "cmd", tp.command,
						       tp.command_end) == FAIL
		    || add_tag_field(dict, "kind", tp.tagkind,
						      tp.tagkind_end) == FAIL
		    || dict_add_number(dict, "static", is_static) == FAIL)
		ret = FAIL;

	    vim_free(full_fname);

	    if (tp.command_end != NULL)
	    {
		for (p = tp.command_end + 3;
			  *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
		{
		    if (p == tp.tagkind || (p + 5 == tp.tagkind
					      && STRNCMP(p, "kind:", 5) == 0))
			// skip "kind:<kind>" and "<kind>"
			p = tp.tagkind_end - 1;
		    else if (STRNCMP(p, "file:", 5) == 0)
			// skip "file:" (static tag)
			p += 4;
		    else if (!VIM_ISWHITE(*p))
		    {
			char_u	*s, *n;
			int	len;

			// Add extra field as a dict entry.  Fields are
			// separated by Tabs.
			n = p;
			while (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			    ++p;
			len = (int)(p - n);
			if (*p == ':' && len > 0)
			{
			    s = ++p;
			    while (*p != NUL && *p >= ' ')
				++p;
			    n[len] = NUL;
			    if (add_tag_field(dict, (char *)n, s, p) == FAIL)
				ret = FAIL;
			    n[len] = ':';
			}
			else
			    // Skip field without colon.
			    while (*p != NUL && *p >= ' ')
				++p;
			if (*p == NUL)
			    break;
		    }
		}
	    }

	    vim_free(matches[i]);
	}
	vim_free(matches);
    }
    return ret;
}