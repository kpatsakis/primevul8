xmlSwitchToEncodingInt(xmlParserCtxtPtr ctxt,
                       xmlCharEncodingHandlerPtr handler, int len) {
    int ret = 0;

    if (handler != NULL) {
        if (ctxt->input != NULL) {
	    ret = xmlSwitchInputEncodingInt(ctxt, ctxt->input, handler, len);
	} else {
	    xmlErrInternal(ctxt, "xmlSwitchToEncoding : no input\n",
	                   NULL);
	    return(-1);
	}
	/*
	 * The parsing is now done in UTF8 natively
	 */
	ctxt->charset = XML_CHAR_ENCODING_UTF8;
    } else
	return(-1);
    return(ret);
}