xmlSaveFormatFileTo(xmlOutputBufferPtr buf, xmlDocPtr cur,
                    const char *encoding, int format)
{
    xmlSaveCtxt ctxt;
    int ret;

    if (buf == NULL) return(-1);
    if ((cur == NULL) ||
        ((cur->type != XML_DOCUMENT_NODE) &&
	 (cur->type != XML_HTML_DOCUMENT_NODE))) {
        xmlOutputBufferClose(buf);
	return(-1);
    }
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
    return (ret);
}