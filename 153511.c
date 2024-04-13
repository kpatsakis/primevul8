process_textarea(struct parsed_tag *tag, int width)
{
    Str tmp = NULL;
    char *p;
#define TEXTAREA_ATTR_COL_MAX 4096
#define TEXTAREA_ATTR_ROWS_MAX 4096

    if (cur_form_id < 0) {
	char *s = "<form_int method=internal action=none>";
	tmp = process_form(parse_tag(&s, TRUE));
    }

    p = "";
    parsedtag_get_value(tag, ATTR_NAME, &p);
    cur_textarea = Strnew_charp(p);
    cur_textarea_size = 20;
    if (parsedtag_get_value(tag, ATTR_COLS, &p)) {
	cur_textarea_size = atoi(p);
	if (p[strlen(p) - 1] == '%')
	    cur_textarea_size = width * cur_textarea_size / 100 - 2;
	if (cur_textarea_size <= 0) {
	    cur_textarea_size = 20;
	} else if (cur_textarea_size > TEXTAREA_ATTR_COL_MAX) {
	    cur_textarea_size = TEXTAREA_ATTR_COL_MAX;
	}
    }
    cur_textarea_rows = 1;
    if (parsedtag_get_value(tag, ATTR_ROWS, &p)) {
	cur_textarea_rows = atoi(p);
	if (cur_textarea_rows <= 0) {
	    cur_textarea_rows = 1;
	} else if (cur_textarea_rows > TEXTAREA_ATTR_ROWS_MAX) {
	    cur_textarea_rows = TEXTAREA_ATTR_ROWS_MAX;
	}
    }
    cur_textarea_readonly = parsedtag_exists(tag, ATTR_READONLY);
    if (n_textarea >= max_textarea) {
	max_textarea *= 2;
	textarea_str = New_Reuse(Str, textarea_str, max_textarea);
    }
    textarea_str[n_textarea] = Strnew();
    ignore_nl_textarea = TRUE;

    return tmp;
}