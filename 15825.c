set_tagstack(win_T *wp, dict_T *d, int action)
{
    dictitem_T	*di;
    list_T	*l = NULL;

#ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    if (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_stack_within_tagfunc));
	return FAIL;
    }
#endif

    if ((di = dict_find(d, (char_u *)"items", -1)) != NULL)
    {
	if (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    return FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    if ((di = dict_find(d, (char_u *)"curidx", -1)) != NULL)
	tagstack_set_curidx(wp, (int)tv_get_number(&di->di_tv) - 1);

    if (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	int	tagstackidx = wp->w_tagstackidx;
	int	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	while (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    if (l != NULL)
    {
	if (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_push_items(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    return OK;
}