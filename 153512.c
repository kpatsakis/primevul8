process_select(struct parsed_tag *tag)
{
    Str tmp = NULL;
    char *p;

    if (cur_form_id < 0) {
	char *s = "<form_int method=internal action=none>";
	tmp = process_form(parse_tag(&s, TRUE));
    }

    p = "";
    parsedtag_get_value(tag, ATTR_NAME, &p);
    cur_select = Strnew_charp(p);
    select_is_multiple = parsedtag_exists(tag, ATTR_MULTIPLE);

#ifdef MENU_SELECT
    if (!select_is_multiple) {
	select_str = Strnew_charp("<pre_int>");
	if (displayLinkNumber)
	    Strcat(select_str, getLinkNumberStr(0));
	Strcat(select_str, Sprintf("[<input_alt hseq=\"%d\" "
			     "fid=\"%d\" type=select name=\"%s\" selectnumber=%d",
			     cur_hseq++, cur_form_id, html_quote(p), n_select));
	Strcat_charp(select_str, ">");
	if (n_select == max_select) {
	    max_select *= 2;
	    select_option =
		New_Reuse(FormSelectOption, select_option, max_select);
	}
	select_option[n_select].first = NULL;
	select_option[n_select].last = NULL;
	cur_option_maxwidth = 0;
    }
    else
#endif				/* MENU_SELECT */
	select_str = Strnew();
    cur_option = NULL;
    cur_status = R_ST_NORMAL;
    n_selectitem = 0;
    return tmp;
}