Suffix(myword, suffix)
    char *myword;
    char *suffix;
{
    register int i;
    register int j;
    i = strlen(myword);
    j = strlen(suffix);

    if (i > j)
    {
	return (STRCMP((myword + i - j), suffix));
    } else
    {
	return (-1);
    }
}