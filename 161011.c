xmlDocFormatDump(FILE *f, xmlDocPtr cur, int format) {
    xmlSaveCtxt ctxt;
    xmlOutputBufferPtr buf;
    const char * encoding;
    xmlCharEncodingHandlerPtr handler = NULL;
    int ret;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlDocDump : document == NULL\n");
#endif
	return(-1);
    }
    encoding = (const char *) cur->encoding;

    if (encoding != NULL) {
	handler = xmlFindCharEncodingHandler(encoding);
	if (handler == NULL) {
	    xmlFree((char *) cur->encoding);
	    cur->encoding = NULL;
	    encoding = NULL;
	}
    }
    buf = xmlOutputBufferCreateFile(f, handler);
    if (buf == NULL) return(-1);
    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = cur;
    ctxt.buf = buf;
    ctxt.level = 0;
    ctxt.format = format ? 1 : 0;
    ctxt.encoding = (const xmlChar *) encoding;
    xmlSaveCtxtInit(&ctxt);
    ctxt.options |= XML_SAVE_AS_XML;
    xmlDocContentDumpOutput(&ctxt, cur);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}