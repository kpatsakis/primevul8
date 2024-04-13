find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    for (;;)
    {
	if (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	else if (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    if (*str != first_char)
		str = NULL;
	    else
		++str;
	}
	else
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((char *)str, "|;\"");
	    if (str != NULL)
	    {
		++str;
		break;
	    }

	}
	if (str == NULL || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    break;
	++str;	// skip ';'
	first_char = *str;
    }

    if (str != NULL && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	return OK;
    }
    return FAIL;
}