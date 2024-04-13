xmlElemDump(FILE * f, xmlDocPtr doc, xmlNodePtr cur)
{
    xmlOutputBufferPtr outbuf;

    xmlInitParser();

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                        "xmlElemDump : cur == NULL\n");
#endif
        return;
    }
#ifdef DEBUG_TREE
    if (doc == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        "xmlElemDump : doc == NULL\n");
    }
#endif

    outbuf = xmlOutputBufferCreateFile(f, NULL);
    if (outbuf == NULL)
        return;
    if ((doc != NULL) && (doc->type == XML_HTML_DOCUMENT_NODE)) {
#ifdef LIBXML_HTML_ENABLED
        htmlNodeDumpOutput(outbuf, doc, cur, NULL);
#else
	xmlSaveErr(XML_ERR_INTERNAL_ERROR, cur, "HTML support not compiled in\n");
#endif /* LIBXML_HTML_ENABLED */
    } else
        xmlNodeDumpOutput(outbuf, doc, cur, 0, 1, NULL);
    xmlOutputBufferClose(outbuf);
}