process_n_title(struct parsed_tag *tag)
{
    Str tmp;

    if (!cur_title)
	return NULL;
    Strremovefirstspaces(cur_title);
    Strremovetrailingspaces(cur_title);
    tmp = Strnew_m_charp("<title_alt title=\"",
			 html_quote(cur_title->ptr), "\">", NULL);
    cur_title = NULL;
    return tmp;
}