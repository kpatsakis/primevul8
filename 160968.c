htmlNodeDumpOutputInternal(xmlSaveCtxtPtr ctxt, xmlNodePtr cur) {
    const xmlChar *oldenc = NULL;
    const xmlChar *oldctxtenc = ctxt->encoding;
    const xmlChar *encoding = ctxt->encoding;
    xmlOutputBufferPtr buf = ctxt->buf;
    int switched_encoding = 0;
    xmlDocPtr doc;

    xmlInitParser();

    doc = cur->doc;
    if (doc != NULL) {
        oldenc = doc->encoding;
	if (ctxt->encoding != NULL) {
	    doc->encoding = BAD_CAST ctxt->encoding;
	} else if (doc->encoding != NULL) {
	    encoding = doc->encoding;
	}
    }

    if ((encoding != NULL) && (doc != NULL))
	htmlSetMetaEncoding(doc, (const xmlChar *) encoding);
    if ((encoding == NULL) && (doc != NULL))
	encoding = htmlGetMetaEncoding(doc);
    if (encoding == NULL)
	encoding = BAD_CAST "HTML";
    if ((encoding != NULL) && (oldctxtenc == NULL) &&
	(buf->encoder == NULL) && (buf->conv == NULL)) {
	if (xmlSaveSwitchEncoding(ctxt, (const char*) encoding) < 0) {
	    doc->encoding = oldenc;
	    return(-1);
	}
	switched_encoding = 1;
    }
    if (ctxt->options & XML_SAVE_FORMAT)
	htmlNodeDumpFormatOutput(buf, doc, cur,
				       (const char *)encoding, 1);
    else
	htmlNodeDumpFormatOutput(buf, doc, cur,
				       (const char *)encoding, 0);
    /*
     * Restore the state of the saving context at the end of the document
     */
    if ((switched_encoding) && (oldctxtenc == NULL)) {
	xmlSaveClearEncoding(ctxt);
    }
    if (doc != NULL)
	doc->encoding = oldenc;
    return(0);
}