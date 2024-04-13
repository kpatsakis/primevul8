checkHeader(Buffer *buf, char *field)
{
    int len;
    TextListItem *i;
    char *p;

    if (buf == NULL || field == NULL || buf->document_header == NULL)
	return NULL;
    len = strlen(field);
    for (i = buf->document_header->first; i != NULL; i = i->next) {
	if (!strncasecmp(i->ptr, field, len)) {
	    p = i->ptr + len;
	    return remove_space(p);
	}
    }
    return NULL;
}