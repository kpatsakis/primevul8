Pluralise(string)		/* returns a pointer to a plural */
    register char *string;
{
    register int length;
    static char area[STRINGSIZE];
    length = strlen(string);
    strcpy(area, string);

    if (!Suffix(string, "ch") ||
	!Suffix(string, "ex") ||
	!Suffix(string, "ix") ||
	!Suffix(string, "sh") ||
	!Suffix(string, "ss"))
    {
	/* bench -> benches */
	strcat(area, "es");
    } else if (length > 2 && string[length - 1] == 'y')
    {
	if (strchr("aeiou", string[length - 2]))
	{
	    /* alloy -> alloys */
	    strcat(area, "s");
	} else
	{
	    /* gully -> gullies */
	    strcpy(area + length - 1, "ies");
	}
    } else if (string[length - 1] == 's')
    {
	/* bias -> biases */
	strcat(area, "es");
    } else
    {
	/* catchall */
	strcat(area, "s");
    }

    return (area);
}