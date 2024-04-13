htmlCreateDocParserCtxt(const xmlChar *cur, const char *encoding) {
    int len;
    htmlParserCtxtPtr ctxt;

    if (cur == NULL)
	return(NULL);
    len = xmlStrlen(cur);
    ctxt = htmlCreateMemoryParserCtxt((char *)cur, len);
    if (ctxt == NULL)
	return(NULL);

    if (encoding != NULL) {
	xmlCharEncoding enc;
	xmlCharEncodingHandlerPtr handler;

	if (ctxt->input->encoding != NULL)
	    xmlFree((xmlChar *) ctxt->input->encoding);
	ctxt->input->encoding = xmlStrdup((const xmlChar *) encoding);

	enc = xmlParseCharEncoding(encoding);
	/*
	 * registered set of known encodings
	 */
	if (enc != XML_CHAR_ENCODING_ERROR) {
	    xmlSwitchEncoding(ctxt, enc);
	    if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
		htmlParseErr(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
		             "Unsupported encoding %s\n",
			     (const xmlChar *) encoding, NULL);
	    }
	} else {
	    /*
	     * fallback for unknown encodings
	     */
	    handler = xmlFindCharEncodingHandler((const char *) encoding);
	    if (handler != NULL) {
		xmlSwitchToEncoding(ctxt, handler);
	    } else {
		htmlParseErr(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
		             "Unsupported encoding %s\n",
			     (const xmlChar *) encoding, NULL);
	    }
	}
    }
    return(ctxt);
}