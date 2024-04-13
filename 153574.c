sloppy_parse_line(char **str)
{
    if (**str == '<') {
	while (**str && **str != '>')
	    (*str)++;
	if (**str == '>')
	    (*str)++;
	return 1;
    }
    else {
	while (**str && **str != '<')
	    (*str)++;
	return 0;
    }
}