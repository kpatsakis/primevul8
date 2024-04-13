is_html_type(char *type)
{
    return (type && (strcasecmp(type, "text/html") == 0 ||
		     strcasecmp(type, "application/xhtml+xml") == 0));
}