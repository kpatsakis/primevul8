is_word_char(unsigned char *ch)
{
    Lineprop ctype = get_mctype(ch);

#ifdef USE_M17N
    if (ctype & (PC_CTRL | PC_KANJI | PC_UNKNOWN))
	return 0;
    if (ctype & (PC_WCHAR1 | PC_WCHAR2))
	return 1;
#else
    if (ctype == PC_CTRL)
	return 0;
#endif

    if (IS_ALNUM(*ch))
	return 1;

    switch (*ch) {
    case ',':
    case '.':
    case ':':
    case '\"':			/* " */
    case '\'':
    case '$':
    case '%':
    case '*':
    case '+':
    case '-':
    case '@':
    case '~':
    case '_':
	return 1;
    }
#ifdef USE_M17N
    if (*ch == NBSP_CODE)
	return 1;
#else
    if (*ch == TIMES_CODE || *ch == DIVIDE_CODE || *ch == ANSP_CODE)
	return 0;
    if (*ch >= AGRAVE_CODE || *ch == NBSP_CODE)
	return 1;
#endif
    return 0;
}