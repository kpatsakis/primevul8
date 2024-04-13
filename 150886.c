xmlValidateDocument(xmlValidCtxtPtr ctxt, xmlDocPtr doc) {
    int ret;
    xmlNodePtr root;

    if (doc == NULL)
        return(0);
    if ((doc->intSubset == NULL) && (doc->extSubset == NULL)) {
        xmlErrValid(ctxt, XML_DTD_NO_DTD,
	            "no DTD found!\n", NULL);
	return(0);
    }
    if ((doc->intSubset != NULL) && ((doc->intSubset->SystemID != NULL) ||
	(doc->intSubset->ExternalID != NULL)) && (doc->extSubset == NULL)) {
	xmlChar *sysID;
	if (doc->intSubset->SystemID != NULL) {
	    sysID = xmlBuildURI(doc->intSubset->SystemID,
			doc->URL);
	    if (sysID == NULL) {
	        xmlErrValid(ctxt, XML_DTD_LOAD_ERROR,
			"Could not build URI for external subset \"%s\"\n",
			(const char *) doc->intSubset->SystemID);
		return 0;
	    }
	} else
	    sysID = NULL;
        doc->extSubset = xmlParseDTD(doc->intSubset->ExternalID,
			(const xmlChar *)sysID);
	if (sysID != NULL)
	    xmlFree(sysID);
        if (doc->extSubset == NULL) {
	    if (doc->intSubset->SystemID != NULL) {
		xmlErrValid(ctxt, XML_DTD_LOAD_ERROR,
		       "Could not load the external subset \"%s\"\n",
		       (const char *) doc->intSubset->SystemID);
	    } else {
		xmlErrValid(ctxt, XML_DTD_LOAD_ERROR,
		       "Could not load the external subset \"%s\"\n",
		       (const char *) doc->intSubset->ExternalID);
	    }
	    return(0);
	}
    }

    if (doc->ids != NULL) {
          xmlFreeIDTable(doc->ids);
          doc->ids = NULL;
    }
    if (doc->refs != NULL) {
          xmlFreeRefTable(doc->refs);
          doc->refs = NULL;
    }
    ret = xmlValidateDtdFinal(ctxt, doc);
    if (!xmlValidateRoot(ctxt, doc)) return(0);

    root = xmlDocGetRootElement(doc);
    ret &= xmlValidateElement(ctxt, doc, root);
    ret &= xmlValidateDocumentFinal(ctxt, doc);
    return(ret);
}