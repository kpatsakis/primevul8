xhtmlAttrListDumpOutput(xmlSaveCtxtPtr ctxt, xmlAttrPtr cur) {
    xmlAttrPtr xml_lang = NULL;
    xmlAttrPtr lang = NULL;
    xmlAttrPtr name = NULL;
    xmlAttrPtr id = NULL;
    xmlNodePtr parent;
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    parent = cur->parent;
    while (cur != NULL) {
	if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "id")))
	    id = cur;
	else
	if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "name")))
	    name = cur;
	else
	if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "lang")))
	    lang = cur;
	else
	if ((cur->ns != NULL) && (xmlStrEqual(cur->name, BAD_CAST "lang")) &&
	    (xmlStrEqual(cur->ns->prefix, BAD_CAST "xml")))
	    xml_lang = cur;
	else if ((cur->ns == NULL) &&
		 ((cur->children == NULL) ||
		  (cur->children->content == NULL) ||
		  (cur->children->content[0] == 0)) &&
		 (htmlIsBooleanAttr(cur->name))) {
	    if (cur->children != NULL)
		xmlFreeNode(cur->children);
	    cur->children = xmlNewText(cur->name);
	    if (cur->children != NULL)
		cur->children->parent = (xmlNodePtr) cur;
	}
        xmlAttrDumpOutput(ctxt, cur);
	cur = cur->next;
    }
    /*
     * C.8
     */
    if ((name != NULL) && (id == NULL)) {
	if ((parent != NULL) && (parent->name != NULL) &&
	    ((xmlStrEqual(parent->name, BAD_CAST "a")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "p")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "div")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "img")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "map")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "applet")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "form")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "frame")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "iframe")))) {
	    xmlOutputBufferWrite(buf, 5, " id=\"");
	    xmlAttrSerializeContent(buf, name);
	    xmlOutputBufferWrite(buf, 1, "\"");
	}
    }
    /*
     * C.7.
     */
    if ((lang != NULL) && (xml_lang == NULL)) {
	xmlOutputBufferWrite(buf, 11, " xml:lang=\"");
	xmlAttrSerializeContent(buf, lang);
	xmlOutputBufferWrite(buf, 1, "\"");
    } else
    if ((xml_lang != NULL) && (lang == NULL)) {
	xmlOutputBufferWrite(buf, 7, " lang=\"");
	xmlAttrSerializeContent(buf, xml_lang);
	xmlOutputBufferWrite(buf, 1, "\"");
    }
}