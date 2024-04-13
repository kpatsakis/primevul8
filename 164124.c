htmlParseDocTypeDecl(htmlParserCtxtPtr ctxt) {
    const xmlChar *name;
    xmlChar *ExternalID = NULL;
    xmlChar *URI = NULL;

    /*
     * We know that '<!DOCTYPE' has been detected.
     */
    SKIP(9);

    SKIP_BLANKS;

    /*
     * Parse the DOCTYPE name.
     */
    name = htmlParseName(ctxt);
    if (name == NULL) {
	htmlParseErr(ctxt, XML_ERR_NAME_REQUIRED,
	             "htmlParseDocTypeDecl : no DOCTYPE name !\n",
		     NULL, NULL);
    }
    /*
     * Check that upper(name) == "HTML" !!!!!!!!!!!!!
     */

    SKIP_BLANKS;

    /*
     * Check for SystemID and ExternalID
     */
    URI = htmlParseExternalID(ctxt, &ExternalID);
    SKIP_BLANKS;

    /*
     * We should be at the end of the DOCTYPE declaration.
     */
    if (CUR != '>') {
	htmlParseErr(ctxt, XML_ERR_DOCTYPE_NOT_FINISHED,
	             "DOCTYPE improperly terminated\n", NULL, NULL);
        /* We shouldn't try to resynchronize ... */
    }
    NEXT;

    /*
     * Create or update the document accordingly to the DOCTYPE
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->internalSubset != NULL) &&
	(!ctxt->disableSAX))
	ctxt->sax->internalSubset(ctxt->userData, name, ExternalID, URI);

    /*
     * Cleanup, since we don't use all those identifiers
     */
    if (URI != NULL) xmlFree(URI);
    if (ExternalID != NULL) xmlFree(ExternalID);
}