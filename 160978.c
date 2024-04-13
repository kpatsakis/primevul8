xmlDocDumpFormatMemoryEnc(xmlDocPtr out_doc, xmlChar **doc_txt_ptr,
		int * doc_txt_len, const char * txt_encoding,
		int format) {
    xmlSaveCtxt ctxt;
    int                         dummy = 0;
    xmlOutputBufferPtr          out_buff = NULL;
    xmlCharEncodingHandlerPtr   conv_hdlr = NULL;

    if (doc_txt_len == NULL) {
        doc_txt_len = &dummy;   /*  Continue, caller just won't get length */
    }

    if (doc_txt_ptr == NULL) {
        *doc_txt_len = 0;
        return;
    }

    *doc_txt_ptr = NULL;
    *doc_txt_len = 0;

    if (out_doc == NULL) {
        /*  No document, no output  */
        return;
    }

    /*
     *  Validate the encoding value, if provided.
     *  This logic is copied from xmlSaveFileEnc.
     */

    if (txt_encoding == NULL)
	txt_encoding = (const char *) out_doc->encoding;
    if (txt_encoding != NULL) {
	conv_hdlr = xmlFindCharEncodingHandler(txt_encoding);
	if ( conv_hdlr == NULL ) {
	    xmlSaveErr(XML_SAVE_UNKNOWN_ENCODING, (xmlNodePtr) out_doc,
		       txt_encoding);
	    return;
	}
    }

    if ((out_buff = xmlAllocOutputBuffer(conv_hdlr)) == NULL ) {
        xmlSaveErrMemory("creating buffer");
        return;
    }

    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = out_doc;
    ctxt.buf = out_buff;
    ctxt.level = 0;
    ctxt.format = format ? 1 : 0;
    ctxt.encoding = (const xmlChar *) txt_encoding;
    xmlSaveCtxtInit(&ctxt);
    ctxt.options |= XML_SAVE_AS_XML;
    xmlDocContentDumpOutput(&ctxt, out_doc);
    xmlOutputBufferFlush(out_buff);
    if (out_buff->conv != NULL) {
	*doc_txt_len = xmlBufUse(out_buff->conv);
	*doc_txt_ptr = xmlStrndup(xmlBufContent(out_buff->conv), *doc_txt_len);
    } else {
	*doc_txt_len = xmlBufUse(out_buff->buffer);
	*doc_txt_ptr = xmlStrndup(xmlBufContent(out_buff->buffer),*doc_txt_len);
    }
    (void)xmlOutputBufferClose(out_buff);

    if ((*doc_txt_ptr == NULL) && (*doc_txt_len > 0)) {
        *doc_txt_len = 0;
        xmlSaveErrMemory("creating output");
    }

    return;
}