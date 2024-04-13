xmlSaveFileTo(xmlOutputBufferPtr buf, xmlDocPtr cur, const char *encoding) {
    xmlSaveCtxt ctxt;
    int ret;

    if (buf == NULL) return(-1);
    if (cur == NULL) {
        xmlOutputBufferClose(buf);
	return(-1);
    }
    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = cur;
    ctxt.buf = buf;
    ctxt.level = 0;
    ctxt.format = 0;
    ctxt.encoding = (const xmlChar *) encoding;
    xmlSaveCtxtInit(&ctxt);
    ctxt.options |= XML_SAVE_AS_XML;
    xmlDocContentDumpOutput(&ctxt, cur);
    ret = xmlOutputBufferClose(buf);
    return(ret);
}