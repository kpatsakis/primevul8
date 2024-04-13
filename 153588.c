ul_type(struct parsed_tag *tag, int default_type)
{
    char *p;
    if (parsedtag_get_value(tag, ATTR_TYPE, &p)) {
	if (!strcasecmp(p, "disc"))
	    return (int)'d';
	else if (!strcasecmp(p, "circle"))
	    return (int)'c';
	else if (!strcasecmp(p, "square"))
	    return (int)'s';
    }
    return default_type;
}