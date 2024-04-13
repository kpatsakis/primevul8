PolyStrchr(string, class)
    register char *string;
    register char class;
{
    while (*string)
    {
	if (MatchClass(class, *string))
	{
	    return (string);
	}
	string++;
    }
    return NULL;
}