xmlFreeElement(xmlElementPtr elem) {
    if (elem == NULL) return;
    xmlUnlinkNode((xmlNodePtr) elem);
    xmlFreeDocElementContent(elem->doc, elem->content);
    if (elem->name != NULL)
	xmlFree((xmlChar *) elem->name);
    if (elem->prefix != NULL)
	xmlFree((xmlChar *) elem->prefix);
#ifdef LIBXML_REGEXP_ENABLED
    if (elem->contModel != NULL)
	xmlRegFreeRegexp(elem->contModel);
#endif
    xmlFree(elem);
}