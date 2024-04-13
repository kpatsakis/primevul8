process_n_textarea(void)
{
    Str tmp;
    int i;

    if (cur_textarea == NULL)
	return NULL;

    tmp = Strnew();
    Strcat(tmp, Sprintf("<pre_int>[<input_alt hseq=\"%d\" fid=\"%d\" "
			"type=textarea name=\"%s\" size=%d rows=%d "
			"top_margin=%d textareanumber=%d",
			cur_hseq, cur_form_id,
			html_quote(cur_textarea->ptr),
			cur_textarea_size, cur_textarea_rows,
			cur_textarea_rows - 1, n_textarea));
    if (cur_textarea_readonly)
	Strcat_charp(tmp, " readonly");
    Strcat_charp(tmp, "><u>");
    for (i = 0; i < cur_textarea_size; i++)
	Strcat_char(tmp, ' ');
    Strcat_charp(tmp, "</u></input_alt>]</pre_int>\n");
    cur_hseq++;
    n_textarea++;
    cur_textarea = NULL;

    return tmp;
}