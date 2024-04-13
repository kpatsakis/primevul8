is_plain_text_type(char *type)
{
    return ((type && strcasecmp(type, "text/plain") == 0) ||
	    (is_text_type(type) && !is_dump_text_type(type)));
}