find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    int		*match_count,	// here the number of tags found will be placed
    int		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    int		ntags = 0;
    int		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    if (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == NULL
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	return FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    if ((d = dict_alloc_lock(VAR_FIXED)) == NULL)
	return FAIL;
    if (tag->user_data != NULL)
	dict_add_string(d, "user_data", tag->user_data);
    if (buf_ffname != NULL)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((char *)flagString, sizeof(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    --d->dv_refcount;

    if (result == FAIL)
	return FAIL;
    if (rettv.v_type == VAR_SPECIAL && rettv.vval.v_number == VVAL_NULL)
    {
	clear_tv(&rettv);
	return NOTDONE;
    }
    if (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_value_from_tagfunc));
	return FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_ITEMS(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	int		len;
	dict_iterator_T	iter;
	char_u		*dict_key;
	typval_T	*tv;
	int		has_extra = 0;
	int		name_only = flags & TAG_NAMES;

	if (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_value_from_tagfunc));
	    break;
	}

#ifdef FEAT_EMACS_TAGS
	len = 3;
#else
	len = 2;
#endif
	res_name = NULL;
	res_fname = NULL;
	res_cmd = NULL;
	res_kind = NULL;

	dict_iterate_start(&item->li_tv, &iter);
	while (NULL != (dict_key = dict_iterate_next(&iter, &tv)))
	{
	    if (tv->v_type != VAR_STRING || tv->vval.v_string == NULL)
		continue;

	    len += (int)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    if (!STRCMP(dict_key, "name"))
	    {
		res_name = tv->vval.v_string;
		continue;
	    }
	    if (!STRCMP(dict_key, "filename"))
	    {
		res_fname = tv->vval.v_string;
		continue;
	    }
	    if (!STRCMP(dict_key, "cmd"))
	    {
		res_cmd = tv->vval.v_string;
		continue;
	    }
	    has_extra = 1;
	    if (!STRCMP(dict_key, "kind"))
	    {
		res_kind = tv->vval.v_string;
		continue;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (int)STRLEN(dict_key) + 1;
	}

	if (has_extra)
	    len += 2;	// need space for ;"

	if (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_value_from_tagfunc));
	    break;
	}

	if (name_only)
	    mfp = vim_strsave(res_name);
	else
	    mfp = alloc(sizeof(char_u) + len + 1);

	if (mfp == NULL)
	    continue;

	if (!name_only)
	{
	    char_u *p = mfp;

	    *p++ = MT_GL_OTH + 1;   // mtt
	    *p++ = TAG_SEP;	    // no tag file name
#ifdef FEAT_EMACS_TAGS
	    *p++ = TAG_SEP;
#endif

	    STRCPY(p, res_name);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_fname);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_cmd);
	    p += STRLEN(p);

	    if (has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		if (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		while (NULL != (dict_key = dict_iterate_next(&iter, &tv)))
		{
		    if (tv->v_type != VAR_STRING || tv->vval.v_string == NULL)
			continue;

		    if (!STRCMP(dict_key, "name"))
			continue;
		    if (!STRCMP(dict_key, "filename"))
			continue;
		    if (!STRCMP(dict_key, "cmd"))
			continue;
		    if (!STRCMP(dict_key, "kind"))
			continue;

		    *p++ = TAB;
		    STRCPY(p, dict_key);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	if (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	else
	{
	    vim_free(mfp);
	    break;
	}
    }

    clear_tv(&rettv);

    *match_count = ntags;
    return result;
}