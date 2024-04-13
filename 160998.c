xmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur,
                  int level, int format, const char *encoding)
{
    xmlSaveCtxt ctxt;
#ifdef LIBXML_HTML_ENABLED
    xmlDtdPtr dtd;
    int is_xhtml = 0;
#endif

    xmlInitParser();

    if ((buf == NULL) || (cur == NULL)) return;

    if (encoding == NULL)
        encoding = "UTF-8";

    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = doc;
    ctxt.buf = buf;
    ctxt.level = level;
    ctxt.format = format ? 1 : 0;
    ctxt.encoding = (const xmlChar *) encoding;
    xmlSaveCtxtInit(&ctxt);
    ctxt.options |= XML_SAVE_AS_XML;

#ifdef LIBXML_HTML_ENABLED
    dtd = xmlGetIntSubset(doc);
    if (dtd != NULL) {
	is_xhtml = xmlIsXHTML(dtd->SystemID, dtd->ExternalID);
	if (is_xhtml < 0)
	    is_xhtml = 0;
    }

    if (is_xhtml)
        xhtmlNodeDumpOutput(&ctxt, cur);
    else
#endif
        xmlNodeDumpOutputInternal(&ctxt, cur);
}