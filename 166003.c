PolySubst(string, class, new)	/* returns pointer to a swapped about copy */
    register char *string;
    register char class;
    register char new;
{
    register char *ptr;
    static char area[STRINGSIZE];
    ptr = area;
    while (*string)
    {
	*(ptr++) = (MatchClass(class, *string) ? new : *string);
	string++;
    }
    *ptr = '\0';
    return (area);
}