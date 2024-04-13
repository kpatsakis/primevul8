htmlSAXParseFile(const char *filename, const char *encoding, htmlSAXHandlerPtr sax,
                 void *userData) {
    htmlDocPtr ret;
    htmlParserCtxtPtr ctxt;
    htmlSAXHandlerPtr oldsax = NULL;

    xmlInitParser();

    ctxt = htmlCreateFileParserCtxt(filename, encoding);
    if (ctxt == NULL) return(NULL);
    if (sax != NULL) {
	oldsax = ctxt->sax;
        ctxt->sax = sax;
        ctxt->userData = userData;
    }

    htmlParseDocument(ctxt);

    ret = ctxt->myDoc;
    if (sax != NULL) {
        ctxt->sax = oldsax;
        ctxt->userData = NULL;
    }
    htmlFreeParserCtxt(ctxt);

    return(ret);
}