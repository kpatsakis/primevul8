xmlCreateEnumeration(const xmlChar *name) {
    xmlEnumerationPtr ret;

    ret = (xmlEnumerationPtr) xmlMalloc(sizeof(xmlEnumeration));
    if (ret == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
        return(NULL);
    }
    memset(ret, 0, sizeof(xmlEnumeration));

    if (name != NULL)
        ret->name = xmlStrdup(name);
    return(ret);
}