Substitute(string, old, new)	/* returns pointer to a swapped about copy */
    register char *string;
    register char old;
    register char new;
{
    register char *ptr;
    static char area[STRINGSIZE];
    ptr = area;
    while (*string)
    {
	*(ptr++) = (*string == old ? new : *string);
	string++;
    }
    *ptr = '\0';
    return (area);
}