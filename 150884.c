xmlValidateDocumentFinal(xmlValidCtxtPtr ctxt, xmlDocPtr doc) {
    xmlRefTablePtr table;
    unsigned int save;

    if (ctxt == NULL)
        return(0);
    if (doc == NULL) {
        xmlErrValid(ctxt, XML_DTD_NO_DOC,
		"xmlValidateDocumentFinal: doc == NULL\n", NULL);
	return(0);
    }

    /* trick to get correct line id report */
    save = ctxt->finishDtd;
    ctxt->finishDtd = 0;

    /*
     * Check all the NOTATION/NOTATIONS attributes
     */
    /*
     * Check all the ENTITY/ENTITIES attributes definition for validity
     */
    /*
     * Check all the IDREF/IDREFS attributes definition for validity
     */
    table = (xmlRefTablePtr) doc->refs;
    ctxt->doc = doc;
    ctxt->valid = 1;
    xmlHashScan(table, (xmlHashScanner) xmlValidateCheckRefCallback, ctxt);

    ctxt->finishDtd = save;
    return(ctxt->valid);
}