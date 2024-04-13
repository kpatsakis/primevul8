xmlCopyAttribute(xmlAttributePtr attr) {
    xmlAttributePtr cur;

    cur = (xmlAttributePtr) xmlMalloc(sizeof(xmlAttribute));
    if (cur == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlAttribute));
    cur->type = XML_ATTRIBUTE_DECL;
    cur->atype = attr->atype;
    cur->def = attr->def;
    cur->tree = xmlCopyEnumeration(attr->tree);
    if (attr->elem != NULL)
	cur->elem = xmlStrdup(attr->elem);
    if (attr->name != NULL)
	cur->name = xmlStrdup(attr->name);
    if (attr->prefix != NULL)
	cur->prefix = xmlStrdup(attr->prefix);
    if (attr->defaultValue != NULL)
	cur->defaultValue = xmlStrdup(attr->defaultValue);
    return(cur);
}