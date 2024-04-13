xmlNewDocElementContent(xmlDocPtr doc, const xmlChar *name,
                        xmlElementContentType type) {
    xmlElementContentPtr ret;
    xmlDictPtr dict = NULL;

    if (doc != NULL)
        dict = doc->dict;

    switch(type) {
	case XML_ELEMENT_CONTENT_ELEMENT:
	    if (name == NULL) {
	        xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR,
			"xmlNewElementContent : name == NULL !\n",
			NULL);
	    }
	    break;
        case XML_ELEMENT_CONTENT_PCDATA:
	case XML_ELEMENT_CONTENT_SEQ:
	case XML_ELEMENT_CONTENT_OR:
	    if (name != NULL) {
	        xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR,
			"xmlNewElementContent : name != NULL !\n",
			NULL);
	    }
	    break;
	default:
	    xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR,
		    "Internal: ELEMENT content corrupted invalid type\n",
		    NULL);
	    return(NULL);
    }
    ret = (xmlElementContentPtr) xmlMalloc(sizeof(xmlElementContent));
    if (ret == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlElementContent));
    ret->type = type;
    ret->ocur = XML_ELEMENT_CONTENT_ONCE;
    if (name != NULL) {
        int l;
	const xmlChar *tmp;

	tmp = xmlSplitQName3(name, &l);
	if (tmp == NULL) {
	    if (dict == NULL)
		ret->name = xmlStrdup(name);
	    else
	        ret->name = xmlDictLookup(dict, name, -1);
	} else {
	    if (dict == NULL) {
		ret->prefix = xmlStrndup(name, l);
		ret->name = xmlStrdup(tmp);
	    } else {
	        ret->prefix = xmlDictLookup(dict, name, l);
		ret->name = xmlDictLookup(dict, tmp, -1);
	    }
	}
    }
    return(ret);
}