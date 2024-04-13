xmlFreeDocElementContent(xmlDocPtr doc, xmlElementContentPtr cur) {
    xmlElementContentPtr next;
    xmlDictPtr dict = NULL;

    if (doc != NULL)
        dict = doc->dict;

    while (cur != NULL) {
        next = cur->c2;
	switch (cur->type) {
	    case XML_ELEMENT_CONTENT_PCDATA:
	    case XML_ELEMENT_CONTENT_ELEMENT:
	    case XML_ELEMENT_CONTENT_SEQ:
	    case XML_ELEMENT_CONTENT_OR:
		break;
	    default:
		xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR,
			"Internal: ELEMENT content corrupted invalid type\n",
			NULL);
		return;
	}
	if (cur->c1 != NULL) xmlFreeDocElementContent(doc, cur->c1);
	if (dict) {
	    if ((cur->name != NULL) && (!xmlDictOwns(dict, cur->name)))
	        xmlFree((xmlChar *) cur->name);
	    if ((cur->prefix != NULL) && (!xmlDictOwns(dict, cur->prefix)))
	        xmlFree((xmlChar *) cur->prefix);
	} else {
	    if (cur->name != NULL) xmlFree((xmlChar *) cur->name);
	    if (cur->prefix != NULL) xmlFree((xmlChar *) cur->prefix);
	}
	xmlFree(cur);
	cur = next;
    }
}