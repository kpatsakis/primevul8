Reverse(str)			/* return a pointer to a reversal */
    register char *str;
{
    register int i;
    register int j;
    static char area[STRINGSIZE];
    j = i = strlen(str);
    while (*str)
    {
	area[--i] = *str++;
    }
    area[j] = '\0';
    return (area);
}