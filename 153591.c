process_option(void)
{
    char begin_char = '[', end_char = ']';
    int len;

    if (cur_select == NULL || cur_option == NULL)
	return;
    while (cur_option->length > 0 && IS_SPACE(Strlastchar(cur_option)))
	Strshrink(cur_option, 1);
    if (cur_option_value == NULL)
	cur_option_value = cur_option;
    if (cur_option_label == NULL)
	cur_option_label = cur_option;
#ifdef MENU_SELECT
    if (!select_is_multiple) {
	len = get_Str_strwidth(cur_option_label);
	if (len > cur_option_maxwidth)
	    cur_option_maxwidth = len;
	addSelectOption(&select_option[n_select],
			cur_option_value,
			cur_option_label, cur_option_selected);
	return;
    }
#endif				/* MENU_SELECT */
    if (!select_is_multiple) {
	begin_char = '(';
	end_char = ')';
    }
    Strcat(select_str, Sprintf("<br><pre_int>%c<input_alt hseq=\"%d\" "
			       "fid=\"%d\" type=%s name=\"%s\" value=\"%s\"",
			       begin_char, cur_hseq++, cur_form_id,
			       select_is_multiple ? "checkbox" : "radio",
			       html_quote(cur_select->ptr),
			       html_quote(cur_option_value->ptr)));
    if (cur_option_selected)
	Strcat_charp(select_str, " checked>*</input_alt>");
    else
	Strcat_charp(select_str, "> </input_alt>");
    Strcat_char(select_str, end_char);
    Strcat_charp(select_str, html_quote(cur_option_label->ptr));
    Strcat_charp(select_str, "</pre_int>");
    n_selectitem++;
}