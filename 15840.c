get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    int		i;
    dict_T	*d;

    dict_add_number(retdict, "length", wp->w_tagstacklen);
    dict_add_number(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_items);
    if (l == NULL)
	return;
    dict_add_list(retdict, "items", l);

    for (i = 0; i < wp->w_tagstacklen; i++)
    {
	if ((d = dict_alloc_id(aid_tagstack_details)) == NULL)
	    return;
	list_append_dict(l, d);

	get_tag_details(&wp->w_tagstack[i], d);
    }
}