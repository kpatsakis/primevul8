Lowercase(str)			/* return a pointer to an lowercase */
    register char *str;
{
    register char *ptr;
    static char area[STRINGSIZE];
    ptr = area;
    while (*str)
    {
	*(ptr++) = CRACK_TOLOWER(*str);
	str++;
    }
    *ptr = '\0';

    return (area);
}