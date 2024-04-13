PMatch(control, string)
register char *control;
register char *string;
{
    while (*string && *control)
    {
    	if (!MatchClass(*control, *string))
    	{
    	    return(0);
    	}

    	string++;
    	control++;
    }

    if (*string || *control)
    {
    	return(0);
    }

    return(1);
}