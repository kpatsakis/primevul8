xmlDocContentDumpOutput(xmlSaveCtxtPtr ctxt, xmlDocPtr cur) {
#ifdef LIBXML_HTML_ENABLED
    xmlDtdPtr dtd;
    int is_xhtml = 0;
#endif
    const xmlChar *oldenc = cur->encoding;
    const xmlChar *oldctxtenc = ctxt->encoding;
    const xmlChar *encoding = ctxt->encoding;
    xmlCharEncodingOutputFunc oldescape = ctxt->escape;
    xmlCharEncodingOutputFunc oldescapeAttr = ctxt->escapeAttr;
    xmlOutputBufferPtr buf = ctxt->buf;
    xmlCharEncoding enc;
    int switched_encoding = 0;

    xmlInitParser();

    if ((cur->type != XML_HTML_DOCUMENT_NODE) &&
        (cur->type != XML_DOCUMENT_NODE))
	 return(-1);

    if (ctxt->encoding != NULL) {
        cur->encoding = BAD_CAST ctxt->encoding;
    } else if (cur->encoding != NULL) {
	encoding = cur->encoding;
    } else if (cur->charset != XML_CHAR_ENCODING_UTF8) {
	encoding = (const xmlChar *)
		     xmlGetCharEncodingName((xmlCharEncoding) cur->charset);
    }

    if (((cur->type == XML_HTML_DOCUMENT_NODE) &&
         ((ctxt->options & XML_SAVE_AS_XML) == 0) &&
         ((ctxt->options & XML_SAVE_XHTML) == 0)) ||
        (ctxt->options & XML_SAVE_AS_HTML)) {
#ifdef LIBXML_HTML_ENABLED
        if (encoding != NULL)
	    htmlSetMetaEncoding(cur, (const xmlChar *) encoding);
        if (encoding == NULL)
	    encoding = htmlGetMetaEncoding(cur);
        if (encoding == NULL)
	    encoding = BAD_CAST "HTML";
	if ((encoding != NULL) && (oldctxtenc == NULL) &&
	    (buf->encoder == NULL) && (buf->conv == NULL)) {
	    if (xmlSaveSwitchEncoding(ctxt, (const char*) encoding) < 0) {
		cur->encoding = oldenc;
		return(-1);
	    }
	}
        if (ctxt->options & XML_SAVE_FORMAT)
	    htmlDocContentDumpFormatOutput(buf, cur,
	                                   (const char *)encoding, 1);
	else
	    htmlDocContentDumpFormatOutput(buf, cur,
	                                   (const char *)encoding, 0);
	if (ctxt->encoding != NULL)
	    cur->encoding = oldenc;
	return(0);
#else
        return(-1);
#endif
    } else if ((cur->type == XML_DOCUMENT_NODE) ||
               (ctxt->options & XML_SAVE_AS_XML) ||
               (ctxt->options & XML_SAVE_XHTML)) {
	enc = xmlParseCharEncoding((const char*) encoding);
	if ((encoding != NULL) && (oldctxtenc == NULL) &&
	    (buf->encoder == NULL) && (buf->conv == NULL) &&
	    ((ctxt->options & XML_SAVE_NO_DECL) == 0)) {
	    if ((enc != XML_CHAR_ENCODING_UTF8) &&
		(enc != XML_CHAR_ENCODING_NONE) &&
		(enc != XML_CHAR_ENCODING_ASCII)) {
		/*
		 * we need to switch to this encoding but just for this
		 * document since we output the XMLDecl the conversion
		 * must be done to not generate not well formed documents.
		 */
		if (xmlSaveSwitchEncoding(ctxt, (const char*) encoding) < 0) {
		    cur->encoding = oldenc;
		    return(-1);
		}
		switched_encoding = 1;
	    }
	    if (ctxt->escape == xmlEscapeEntities)
		ctxt->escape = NULL;
	    if (ctxt->escapeAttr == xmlEscapeEntities)
		ctxt->escapeAttr = NULL;
	}


	/*
	 * Save the XML declaration
	 */
	if ((ctxt->options & XML_SAVE_NO_DECL) == 0) {
	    xmlOutputBufferWrite(buf, 14, "<?xml version=");
	    if (cur->version != NULL)
		xmlBufWriteQuotedString(buf->buffer, cur->version);
	    else
		xmlOutputBufferWrite(buf, 5, "\"1.0\"");
	    if (encoding != NULL) {
		xmlOutputBufferWrite(buf, 10, " encoding=");
		xmlBufWriteQuotedString(buf->buffer, (xmlChar *) encoding);
	    }
	    switch (cur->standalone) {
		case 0:
		    xmlOutputBufferWrite(buf, 16, " standalone=\"no\"");
		    break;
		case 1:
		    xmlOutputBufferWrite(buf, 17, " standalone=\"yes\"");
		    break;
	    }
	    xmlOutputBufferWrite(buf, 3, "?>\n");
	}

#ifdef LIBXML_HTML_ENABLED
        if (ctxt->options & XML_SAVE_XHTML)
            is_xhtml = 1;
	if ((ctxt->options & XML_SAVE_NO_XHTML) == 0) {
	    dtd = xmlGetIntSubset(cur);
	    if (dtd != NULL) {
		is_xhtml = xmlIsXHTML(dtd->SystemID, dtd->ExternalID);
		if (is_xhtml < 0) is_xhtml = 0;
	    }
	}
#endif
	if (cur->children != NULL) {
	    xmlNodePtr child = cur->children;

	    while (child != NULL) {
		ctxt->level = 0;
#ifdef LIBXML_HTML_ENABLED
		if (is_xhtml)
		    xhtmlNodeDumpOutput(ctxt, child);
		else
#endif
		    xmlNodeDumpOutputInternal(ctxt, child);
		xmlOutputBufferWrite(buf, 1, "\n");
		child = child->next;
	    }
	}
    }

    /*
     * Restore the state of the saving context at the end of the document
     */
    if ((switched_encoding) && (oldctxtenc == NULL)) {
	xmlSaveClearEncoding(ctxt);
	ctxt->escape = oldescape;
	ctxt->escapeAttr = oldescapeAttr;
    }
    cur->encoding = oldenc;
    return(0);
}