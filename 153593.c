feed_textarea(char *str)
{
    if (cur_textarea == NULL)
	return;
    if (ignore_nl_textarea) {
	if (*str == '\r')
	    str++;
	if (*str == '\n')
	    str++;
    }
    ignore_nl_textarea = FALSE;
    while (*str) {
	if (*str == '&')
	    Strcat_charp(textarea_str[n_textarea], getescapecmd(&str));
	else if (*str == '\n') {
	    Strcat_charp(textarea_str[n_textarea], "\r\n");
	    str++;
	}
	else if (*str != '\r')
	    Strcat_char(textarea_str[n_textarea], *(str++));
    }
}