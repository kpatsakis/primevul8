feed_title(char *str)
{
    if (!cur_title)
	return;
    while (*str) {
	if (*str == '&')
	    Strcat_charp(cur_title, getescapecmd(&str));
	else if (*str == '\n' || *str == '\r') {
	    Strcat_char(cur_title, ' ');
	    str++;
	}
	else
	    Strcat_char(cur_title, *(str++));
    }
}