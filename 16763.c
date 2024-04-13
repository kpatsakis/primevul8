eval_vars(
    char_u	*src,		// pointer into commandline
    char_u	*srcstart,	// beginning of valid memory for src
    int		*usedlen,	// characters after src that are used
    linenr_T	*lnump,		// line number for :e command, or NULL
    char	**errormsg,	// pointer to error message
    int		*escaped)	// return value has escaped white space (can
				// be NULL)
{
    int		i;
    char_u	*s;
    char_u	*result;
    char_u	*resultbuf = NULL;
    int		resultlen;
    buf_T	*buf;
    int		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    int		spec_idx;
    int		tilde_file = FALSE;
    int		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = NULL;
    if (escaped != NULL)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    if (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	return NULL;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    if (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	return NULL;
    }

    /*
     * word or WORD under cursor
     */
    if (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	if (resultlen == 0)
	{
	    *errormsg = "";
	    return NULL;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    else
    {
	int off = 0;

	switch (spec_idx)
	{
	case SPEC_PERC:
#ifdef FEAT_EVAL
		if (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    if (curbuf->b_fname == NULL)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    else
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    break;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	case SPEC_HASH:		// '#' or "#99": alternate file
		if (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    if (escaped != NULL)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    break;
		}
		s = src + off + 1;
		if (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (int)getdigits(&s);
		if (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (int)(s - src); // length of what we expand

		if (src[off + 1] == '<' && i != 0)
		{
		    if (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			return NULL;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    if (result == NULL)
		    {
			*errormsg = "";
			return NULL;
		    }
#else
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    return NULL;
#endif
		}
		else
		{
		    if (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    if (buf == NULL)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			return NULL;
		    }
		    if (lnump != NULL)
			*lnump = ECMD_LAST;
		    if (buf->b_fname == NULL)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    else
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		break;

#ifdef FEAT_SEARCHPATH
	case SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, NULL);
		if (result == NULL)
		{
		    *errormsg = "";
		    return NULL;
		}
		resultbuf = result;	    // remember allocated string
		break;
#endif

	case SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		if (result != NULL && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		if (result == NULL)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    return NULL;
		}
		result = shorten_fname1(result);
		break;

	case SPEC_ABUF:		// buffer number for autocommand
		if (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_name_to_substitute_for_abuf);
		    return NULL;
		}
		sprintf((char *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		break;

	case SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		if (result == NULL)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    return NULL;
		}
		break;

	case SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		if (result == NULL)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    return NULL;
		}
		resultbuf = result;	    // remember allocated string
		break;
	case SPEC_STACK:	// call stack
		result = estack_sfile(ESTACK_STACK);
		if (result == NULL)
		{
		    *errormsg = _(e_no_call_stack_to_substitute_for_stack);
		    return NULL;
		}
		resultbuf = result;	    // remember allocated string
		break;
	case SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		if (result == NULL)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    return NULL;
		}
		resultbuf = result;	    // remember allocated string
		break;

	case SPEC_SLNUM:	// line in file for ":so" command
		if (SOURCING_NAME == NULL || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_line_number_to_use_for_slnum);
		    return NULL;
		}
		sprintf((char *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		break;

#ifdef FEAT_EVAL
	case SPEC_SFLNUM:	// line in script file
		if (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_line_number_to_use_for_sflnum);
		    return NULL;
		}
		sprintf((char *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		break;

	case SPEC_SID:
		if (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    return NULL;
		}
		sprintf((char *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		break;
#endif

#ifdef FEAT_CLIENTSERVER
	case SPEC_CLIENT:	// Source of last submitted input
		sprintf((char *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
		break;
#endif

	default:
		result = (char_u *)""; // avoid gcc warning
		break;
	}

	resultlen = (int)STRLEN(result);	// length of new string
	if (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    if ((s = vim_strrchr(result, '.')) != NULL && s >= gettail(result))
		resultlen = (int)(s - result);
	}
	else if (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    if (result == NULL)
	    {
		*errormsg = "";
		return NULL;
	    }
	}
    }

    if (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	if (valid != VALID_HEAD + VALID_PATH)
	    *errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	else
	    *errormsg = _(e_evaluates_to_an_empty_string);
	result = NULL;
    }
    else
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    return result;
}