xmlValidateNotationUse(xmlValidCtxtPtr ctxt, xmlDocPtr doc,
                       const xmlChar *notationName) {
    xmlNotationPtr notaDecl;
    if ((doc == NULL) || (doc->intSubset == NULL) ||
        (notationName == NULL)) return(-1);

    notaDecl = xmlGetDtdNotationDesc(doc->intSubset, notationName);
    if ((notaDecl == NULL) && (doc->extSubset != NULL))
	notaDecl = xmlGetDtdNotationDesc(doc->extSubset, notationName);

    if ((notaDecl == NULL) && (ctxt != NULL)) {
	xmlErrValidNode(ctxt, (xmlNodePtr) doc, XML_DTD_UNKNOWN_NOTATION,
	                "NOTATION %s is not declared\n",
		        notationName, NULL, NULL);
	return(0);
    }
    return(1);
}