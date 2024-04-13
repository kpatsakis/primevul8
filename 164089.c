htmlDoRead(htmlParserCtxtPtr ctxt, const char *URL, const char *encoding,
          int options, int reuse)
{
    htmlDocPtr ret;

    htmlCtxtUseOptions(ctxt, options);
    ctxt->html = 1;
    if (encoding != NULL) {
        xmlCharEncodingHandlerPtr hdlr;

	hdlr = xmlFindCharEncodingHandler(encoding);
	if (hdlr != NULL) {
	    xmlSwitchToEncoding(ctxt, hdlr);
	    if (ctxt->input->encoding != NULL)
	      xmlFree((xmlChar *) ctxt->input->encoding);
            ctxt->input->encoding = xmlStrdup((xmlChar *)encoding);
        }
    }
    if ((URL != NULL) && (ctxt->input != NULL) &&
        (ctxt->input->filename == NULL))
        ctxt->input->filename = (char *) xmlStrdup((const xmlChar *) URL);
    htmlParseDocument(ctxt);
    ret = ctxt->myDoc;
    ctxt->myDoc = NULL;
    if (!reuse) {
        if ((ctxt->dictNames) &&
	    (ret != NULL) &&
	    (ret->dict == ctxt->dict))
	    ctxt->dict = NULL;
	xmlFreeParserCtxt(ctxt);
    }
    return (ret);
}