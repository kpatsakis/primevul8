xmlFreeAttribute(xmlAttributePtr attr) {
    xmlDictPtr dict;

    if (attr == NULL) return;
    if (attr->doc != NULL)
	dict = attr->doc->dict;
    else
	dict = NULL;
    xmlUnlinkNode((xmlNodePtr) attr);
    if (attr->tree != NULL)
        xmlFreeEnumeration(attr->tree);
    if (dict) {
        if ((attr->elem != NULL) && (!xmlDictOwns(dict, attr->elem)))
	    xmlFree((xmlChar *) attr->elem);
        if ((attr->name != NULL) && (!xmlDictOwns(dict, attr->name)))
	    xmlFree((xmlChar *) attr->name);
        if ((attr->prefix != NULL) && (!xmlDictOwns(dict, attr->prefix)))
	    xmlFree((xmlChar *) attr->prefix);
        if ((attr->defaultValue != NULL) &&
	    (!xmlDictOwns(dict, attr->defaultValue)))
	    xmlFree((xmlChar *) attr->defaultValue);
    } else {
	if (attr->elem != NULL)
	    xmlFree((xmlChar *) attr->elem);
	if (attr->name != NULL)
	    xmlFree((xmlChar *) attr->name);
	if (attr->defaultValue != NULL)
	    xmlFree((xmlChar *) attr->defaultValue);
	if (attr->prefix != NULL)
	    xmlFree((xmlChar *) attr->prefix);
    }
    xmlFree(attr);
}