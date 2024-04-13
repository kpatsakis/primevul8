tagstack_push_items(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    int		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_ITEMS(l, li)
    {
	if (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == NULL)
	    continue;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	if ((di = dict_find(itemdict, (char_u *)"from", -1)) == NULL)
	    continue;
	if (list2fpos(&di->di_tv, &mark, &fnum, NULL, FALSE) != OK)
	    continue;
	if ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == NULL)
	    continue;

	if (mark.col > 0)
	    mark.col--;
	tagstack_push_item(wp, tagname,
		(int)dict_get_number(itemdict, "bufnr"),
		(int)dict_get_number(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}