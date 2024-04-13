xmlCopyDocElementContent(xmlDocPtr doc, xmlElementContentPtr cur) {
    xmlElementContentPtr ret = NULL, prev = NULL, tmp;
    xmlDictPtr dict = NULL;

    if (cur == NULL) return(NULL);

    if (doc != NULL)
        dict = doc->dict;

    ret = (xmlElementContentPtr) xmlMalloc(sizeof(xmlElementContent));
    if (ret == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlElementContent));
    ret->type = cur->type;
    ret->ocur = cur->ocur;
    if (cur->name != NULL) {
	if (dict)
	    ret->name = xmlDictLookup(dict, cur->name, -1);
	else
	    ret->name = xmlStrdup(cur->name);
    }

    if (cur->prefix != NULL) {
	if (dict)
	    ret->prefix = xmlDictLookup(dict, cur->prefix, -1);
	else
	    ret->prefix = xmlStrdup(cur->prefix);
    }
    if (cur->c1 != NULL)
        ret->c1 = xmlCopyDocElementContent(doc, cur->c1);
    if (ret->c1 != NULL)
	ret->c1->parent = ret;
    if (cur->c2 != NULL) {
        prev = ret;
	cur = cur->c2;
	while (cur != NULL) {
	    tmp = (xmlElementContentPtr) xmlMalloc(sizeof(xmlElementContent));
	    if (tmp == NULL) {
		xmlVErrMemory(NULL, "malloc failed");
		return(ret);
	    }
	    memset(tmp, 0, sizeof(xmlElementContent));
	    tmp->type = cur->type;
	    tmp->ocur = cur->ocur;
	    prev->c2 = tmp;
	    if (cur->name != NULL) {
		if (dict)
		    tmp->name = xmlDictLookup(dict, cur->name, -1);
		else
		    tmp->name = xmlStrdup(cur->name);
	    }

	    if (cur->prefix != NULL) {
		if (dict)
		    tmp->prefix = xmlDictLookup(dict, cur->prefix, -1);
		else
		    tmp->prefix = xmlStrdup(cur->prefix);
	    }
	    if (cur->c1 != NULL)
	        tmp->c1 = xmlCopyDocElementContent(doc,cur->c1);
	    if (tmp->c1 != NULL)
		tmp->c1->parent = ret;
	    prev = tmp;
	    cur = cur->c2;
	}
    }
    return(ret);
}