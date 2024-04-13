process_n_select(void)
{
    if (cur_select == NULL)
	return NULL;
    process_option();
#ifdef MENU_SELECT
    if (!select_is_multiple) {
	if (select_option[n_select].first) {
	    FormItemList sitem;
	    chooseSelectOption(&sitem, select_option[n_select].first);
	    Strcat(select_str, textfieldrep(sitem.label, cur_option_maxwidth));
	}
	Strcat_charp(select_str, "</input_alt>]</pre_int>");
	n_select++;
    }
    else
#endif				/* MENU_SELECT */
	Strcat_charp(select_str, "<br>");
    cur_select = NULL;
    n_selectitem = 0;
    return select_str;
}