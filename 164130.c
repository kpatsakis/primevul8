htmlSAXParseDoc(xmlChar *cur, const char *encoding, htmlSAXHandlerPtr sax, void *userData) {
    htmlDocPtr ret;
    htmlParserCtxtPtr ctxt;

    xmlInitParser();

    if (cur == NULL) return(NULL);


    ctxt = htmlCreateDocParserCtxt(cur, encoding);
    if (ctxt == NULL) return(NULL);
    if (sax != NULL) {
        if (ctxt->sax != NULL) xmlFree (ctxt->sax);
        ctxt->sax = sax;
        ctxt->userData = userData;
    }

    htmlParseDocument(ctxt);
    ret = ctxt->myDoc;
    if (sax != NULL) {
	ctxt->sax = NULL;
	ctxt->userData = NULL;
    }
    htmlFreeParserCtxt(ctxt);

    return(ret);
}