PolyPurge(string, class)	/* returns pointer to a purged copy */
    register char *string;
    register char class;
{
    register char *ptr;
    static char area[STRINGSIZE];
    ptr = area;
    while (*string)
    {
	if (!MatchClass(class, *string))
	{
	    *(ptr++) = *string;
	}
	string++;
    }
    *ptr = '\0';
    return (area);
}