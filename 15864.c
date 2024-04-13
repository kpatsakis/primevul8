tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    int		i;

    while (len > 0)
    {
	i = (int)TOUPPER_ASC(*s1) - (int)TOUPPER_ASC(*s2);
	if (i != 0)
	    return i;			// this character different
	if (*s1 == NUL)
	    break;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    return 0;				// strings match
}