xmlIsID(xmlDocPtr doc, xmlNodePtr elem, xmlAttrPtr attr) {
    if ((attr == NULL) || (attr->name == NULL)) return(0);
    if ((attr->ns != NULL) && (attr->ns->prefix != NULL) &&
        (!strcmp((char *) attr->name, "id")) &&
        (!strcmp((char *) attr->ns->prefix, "xml")))
	return(1);
    if (doc == NULL) return(0);
    if ((doc->intSubset == NULL) && (doc->extSubset == NULL) &&
        (doc->type != XML_HTML_DOCUMENT_NODE)) {
	return(0);
    } else if (doc->type == XML_HTML_DOCUMENT_NODE) {
        if ((xmlStrEqual(BAD_CAST "id", attr->name)) ||
	    ((xmlStrEqual(BAD_CAST "name", attr->name)) &&
	    ((elem == NULL) || (xmlStrEqual(elem->name, BAD_CAST "a")))))
	    return(1);
	return(0);
    } else if (elem == NULL) {
	return(0);
    } else {
	xmlAttributePtr attrDecl = NULL;

	xmlChar felem[50], fattr[50];
	xmlChar *fullelemname, *fullattrname;

	fullelemname = (elem->ns != NULL && elem->ns->prefix != NULL) ?
	    xmlBuildQName(elem->name, elem->ns->prefix, felem, 50) :
	    (xmlChar *)elem->name;

	fullattrname = (attr->ns != NULL && attr->ns->prefix != NULL) ?
	    xmlBuildQName(attr->name, attr->ns->prefix, fattr, 50) :
	    (xmlChar *)attr->name;

	if (fullelemname != NULL && fullattrname != NULL) {
	    attrDecl = xmlGetDtdAttrDesc(doc->intSubset, fullelemname,
		                         fullattrname);
	    if ((attrDecl == NULL) && (doc->extSubset != NULL))
		attrDecl = xmlGetDtdAttrDesc(doc->extSubset, fullelemname,
					     fullattrname);
	}

	if ((fullattrname != fattr) && (fullattrname != attr->name))
	    xmlFree(fullattrname);
	if ((fullelemname != felem) && (fullelemname != elem->name))
	    xmlFree(fullelemname);

        if ((attrDecl != NULL) && (attrDecl->atype == XML_ATTRIBUTE_ID))
	    return(1);
    }
    return(0);
}