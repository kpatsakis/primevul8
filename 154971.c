xmlNewParserCtxt(void)
{
    xmlParserCtxtPtr ctxt;

    ctxt = (xmlParserCtxtPtr) xmlMalloc(sizeof(xmlParserCtxt));
    if (ctxt == NULL) {
	xmlErrMemory(NULL, "cannot allocate parser context\n");
	return(NULL);
    }
    memset(ctxt, 0, sizeof(xmlParserCtxt));
    if (xmlInitParserCtxt(ctxt) < 0) {
        xmlFreeParserCtxt(ctxt);
	return(NULL);
    }
    return(ctxt);
}