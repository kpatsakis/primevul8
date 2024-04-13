htmlNewParserCtxt(void)
{
    xmlParserCtxtPtr ctxt;

    ctxt = (xmlParserCtxtPtr) xmlMalloc(sizeof(xmlParserCtxt));
    if (ctxt == NULL) {
        htmlErrMemory(NULL, "NewParserCtxt: out of memory\n");
	return(NULL);
    }
    memset(ctxt, 0, sizeof(xmlParserCtxt));
    if (htmlInitParserCtxt(ctxt) < 0) {
        htmlFreeParserCtxt(ctxt);
	return(NULL);
    }
    return(ctxt);
}