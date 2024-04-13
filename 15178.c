next_for_item(void *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    int		result;
    int		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE)
			 : 0);
    listitem_T	*item;
    int		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    if (fi->fi_blob != NULL)
    {
	typval_T	tv;

	if (fi->fi_bi >= blob_len(fi->fi_blob))
	    return FALSE;
	tv.v_type = VAR_NUMBER;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_number = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	if (skip_assign)
	    return TRUE;
	return ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, NULL) == OK;
    }

    if (fi->fi_string != NULL)
    {
	typval_T	tv;
	int		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	if (len == 0)
	    return FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	if (skip_assign)
	    result = TRUE;
	else
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, NULL) == OK;
	vim_free(tv.vval.v_string);
	return result;
    }

    item = fi->fi_lw.lw_item;
    if (item == NULL)
	result = FALSE;
    else
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	if (skip_assign)
	    result = TRUE;
	else
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, NULL) == OK);
    }
    return result;
}