xmlCopyElement(xmlElementPtr elem) {
    xmlElementPtr cur;

    cur = (xmlElementPtr) xmlMalloc(sizeof(xmlElement));
    if (cur == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlElement));
    cur->type = XML_ELEMENT_DECL;
    cur->etype = elem->etype;
    if (elem->name != NULL)
	cur->name = xmlStrdup(elem->name);
    else
	cur->name = NULL;
    if (elem->prefix != NULL)
	cur->prefix = xmlStrdup(elem->prefix);
    else
	cur->prefix = NULL;
    cur->content = xmlCopyElementContent(elem->content);
    /* TODO : rebuild the attribute list on the copy */
    cur->attributes = NULL;
    return(cur);
}