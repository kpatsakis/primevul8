xmlValidGetElemDecl(xmlValidCtxtPtr ctxt, xmlDocPtr doc,
	            xmlNodePtr elem, int *extsubset) {
    xmlElementPtr elemDecl = NULL;
    const xmlChar *prefix = NULL;

    if ((ctxt == NULL) || (doc == NULL) ||
        (elem == NULL) || (elem->name == NULL))
        return(NULL);
    if (extsubset != NULL)
	*extsubset = 0;

    /*
     * Fetch the declaration for the qualified name
     */
    if ((elem->ns != NULL) && (elem->ns->prefix != NULL))
	prefix = elem->ns->prefix;

    if (prefix != NULL) {
	elemDecl = xmlGetDtdQElementDesc(doc->intSubset,
		                         elem->name, prefix);
	if ((elemDecl == NULL) && (doc->extSubset != NULL)) {
	    elemDecl = xmlGetDtdQElementDesc(doc->extSubset,
		                             elem->name, prefix);
	    if ((elemDecl != NULL) && (extsubset != NULL))
		*extsubset = 1;
	}
    }

    /*
     * Fetch the declaration for the non qualified name
     * This is "non-strict" validation should be done on the
     * full QName but in that case being flexible makes sense.
     */
    if (elemDecl == NULL) {
	elemDecl = xmlGetDtdElementDesc(doc->intSubset, elem->name);
	if ((elemDecl == NULL) && (doc->extSubset != NULL)) {
	    elemDecl = xmlGetDtdElementDesc(doc->extSubset, elem->name);
	    if ((elemDecl != NULL) && (extsubset != NULL))
		*extsubset = 1;
	}
    }
    if (elemDecl == NULL) {
	xmlErrValidNode(ctxt, elem,
			XML_DTD_UNKNOWN_ELEM,
	       "No declaration for element %s\n",
	       elem->name, NULL, NULL);
    }
    return(elemDecl);
}