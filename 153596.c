is_combining_char(unsigned char *ch)
{
    Lineprop ctype = get_mctype(ch);

    if (ctype & PC_WCHAR2)
	return 1;
    return 0;
}