xmlValidateRef(xmlRefPtr ref, xmlValidCtxtPtr ctxt,
	                   const xmlChar *name) {
    xmlAttrPtr id;
    xmlAttrPtr attr;

    if (ref == NULL)
	return;
    if ((ref->attr == NULL) && (ref->name == NULL))
	return;
    attr = ref->attr;
    if (attr == NULL) {
	xmlChar *dup, *str = NULL, *cur, save;

	dup = xmlStrdup(name);
	if (dup == NULL) {
	    ctxt->valid = 0;
	    return;
	}
	cur = dup;
	while (*cur != 0) {
	    str = cur;
	    while ((*cur != 0) && (!IS_BLANK_CH(*cur))) cur++;
	    save = *cur;
	    *cur = 0;
	    id = xmlGetID(ctxt->doc, str);
	    if (id == NULL) {
		xmlErrValidNodeNr(ctxt, NULL, XML_DTD_UNKNOWN_ID,
	   "attribute %s line %d references an unknown ID \"%s\"\n",
		       ref->name, ref->lineno, str);
		ctxt->valid = 0;
	    }
	    if (save == 0)
		break;
	    *cur = save;
	    while (IS_BLANK_CH(*cur)) cur++;
	}
	xmlFree(dup);
    } else if (attr->atype == XML_ATTRIBUTE_IDREF) {
	id = xmlGetID(ctxt->doc, name);
	if (id == NULL) {
	    xmlErrValidNode(ctxt, attr->parent, XML_DTD_UNKNOWN_ID,
	   "IDREF attribute %s references an unknown ID \"%s\"\n",
		   attr->name, name, NULL);
	    ctxt->valid = 0;
	}
    } else if (attr->atype == XML_ATTRIBUTE_IDREFS) {
	xmlChar *dup, *str = NULL, *cur, save;

	dup = xmlStrdup(name);
	if (dup == NULL) {
	    xmlVErrMemory(ctxt, "IDREFS split");
	    ctxt->valid = 0;
	    return;
	}
	cur = dup;
	while (*cur != 0) {
	    str = cur;
	    while ((*cur != 0) && (!IS_BLANK_CH(*cur))) cur++;
	    save = *cur;
	    *cur = 0;
	    id = xmlGetID(ctxt->doc, str);
	    if (id == NULL) {
		xmlErrValidNode(ctxt, attr->parent, XML_DTD_UNKNOWN_ID,
	   "IDREFS attribute %s references an unknown ID \"%s\"\n",
			     attr->name, str, NULL);
		ctxt->valid = 0;
	    }
	    if (save == 0)
		break;
	    *cur = save;
	    while (IS_BLANK_CH(*cur)) cur++;
	}
	xmlFree(dup);
    }
}