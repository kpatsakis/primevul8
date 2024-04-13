is_boundary(unsigned char *ch1, unsigned char *ch2)
{
    if (!*ch1 || !*ch2)
	return 1;

    if (*ch1 == ' ' && *ch2 == ' ')
	return 0;

    if (*ch1 != ' ' && is_period_char(ch2))
	return 0;

    if (*ch2 != ' ' && is_beginning_char(ch1))
	return 0;

#ifdef USE_M17N
    if (is_combining_char(ch2))
	return 0;
#endif
    if (is_word_char(ch1) && is_word_char(ch2))
	return 0;

    return 1;
}