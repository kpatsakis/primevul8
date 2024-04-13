xmlIsRef(xmlDocPtr doc, xmlNodePtr elem, xmlAttrPtr attr) {
    if (attr == NULL)
        return(0);
    if (doc == NULL) {
        doc = attr->doc;
	if (doc == NULL) return(0);
    }

    if ((doc->intSubset == NULL) && (doc->extSubset == NULL)) {
        return(0);
    } else if (doc->type == XML_HTML_DOCUMENT_NODE) {
        /* TODO @@@ */
        return(0);
    } else {
        xmlAttributePtr attrDecl;

        if (elem == NULL) return(0);
        attrDecl = xmlGetDtdAttrDesc(doc->intSubset, elem->name, attr->name);
        if ((attrDecl == NULL) && (doc->extSubset != NULL))
            attrDecl = xmlGetDtdAttrDesc(doc->extSubset,
		                         elem->name, attr->name);

	if ((attrDecl != NULL) &&
	    (attrDecl->atype == XML_ATTRIBUTE_IDREF ||
	     attrDecl->atype == XML_ATTRIBUTE_IDREFS))
	return(1);
    }
    return(0);
}