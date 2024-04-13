xmlIsMixedElement(xmlDocPtr doc, const xmlChar *name) {
    xmlElementPtr elemDecl;

    if ((doc == NULL) || (doc->intSubset == NULL)) return(-1);

    elemDecl = xmlGetDtdElementDesc(doc->intSubset, name);
    if ((elemDecl == NULL) && (doc->extSubset != NULL))
	elemDecl = xmlGetDtdElementDesc(doc->extSubset, name);
    if (elemDecl == NULL) return(-1);
    switch (elemDecl->etype) {
	case XML_ELEMENT_TYPE_UNDEFINED:
	    return(-1);
	case XML_ELEMENT_TYPE_ELEMENT:
	    return(0);
        case XML_ELEMENT_TYPE_EMPTY:
	    /*
	     * return 1 for EMPTY since we want VC error to pop up
	     * on <empty>     </empty> for example
	     */
	case XML_ELEMENT_TYPE_ANY:
	case XML_ELEMENT_TYPE_MIXED:
	    return(1);
    }
    return(1);
}