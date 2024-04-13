xmlAddNotationDecl(xmlValidCtxtPtr ctxt, xmlDtdPtr dtd,
	           const xmlChar *name,
                   const xmlChar *PublicID, const xmlChar *SystemID) {
    xmlNotationPtr ret;
    xmlNotationTablePtr table;

    if (dtd == NULL) {
	return(NULL);
    }
    if (name == NULL) {
	return(NULL);
    }
    if ((PublicID == NULL) && (SystemID == NULL)) {
	return(NULL);
    }

    /*
     * Create the Notation table if needed.
     */
    table = (xmlNotationTablePtr) dtd->notations;
    if (table == NULL) {
	xmlDictPtr dict = NULL;
	if (dtd->doc != NULL)
	    dict = dtd->doc->dict;

        dtd->notations = table = xmlHashCreateDict(0, dict);
    }
    if (table == NULL) {
	xmlVErrMemory(ctxt,
		"xmlAddNotationDecl: Table creation failed!\n");
        return(NULL);
    }

    ret = (xmlNotationPtr) xmlMalloc(sizeof(xmlNotation));
    if (ret == NULL) {
	xmlVErrMemory(ctxt, "malloc failed");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlNotation));

    /*
     * fill the structure.
     */
    ret->name = xmlStrdup(name);
    if (SystemID != NULL)
        ret->SystemID = xmlStrdup(SystemID);
    if (PublicID != NULL)
        ret->PublicID = xmlStrdup(PublicID);

    /*
     * Validity Check:
     * Check the DTD for previous declarations of the ATTLIST
     */
    if (xmlHashAddEntry(table, name, ret)) {
#ifdef LIBXML_VALID_ENABLED
	xmlErrValid(NULL, XML_DTD_NOTATION_REDEFINED,
		    "xmlAddNotationDecl: %s already defined\n",
		    (const char *) name);
#endif /* LIBXML_VALID_ENABLED */
	xmlFreeNotation(ret);
	return(NULL);
    }
    return(ret);
}