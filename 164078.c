htmlCreatePushParserCtxt(htmlSAXHandlerPtr sax, void *user_data,
                         const char *chunk, int size, const char *filename,
			 xmlCharEncoding enc) {
    htmlParserCtxtPtr ctxt;
    htmlParserInputPtr inputStream;
    xmlParserInputBufferPtr buf;

    xmlInitParser();

    buf = xmlAllocParserInputBuffer(enc);
    if (buf == NULL) return(NULL);

    ctxt = htmlNewParserCtxt();
    if (ctxt == NULL) {
	xmlFreeParserInputBuffer(buf);
	return(NULL);
    }
    if(enc==XML_CHAR_ENCODING_UTF8 || buf->encoder)
	ctxt->charset=XML_CHAR_ENCODING_UTF8;
    if (sax != NULL) {
	if (ctxt->sax != (xmlSAXHandlerPtr) &htmlDefaultSAXHandler)
	    xmlFree(ctxt->sax);
	ctxt->sax = (htmlSAXHandlerPtr) xmlMalloc(sizeof(htmlSAXHandler));
	if (ctxt->sax == NULL) {
	    xmlFree(buf);
	    xmlFree(ctxt);
	    return(NULL);
	}
	memcpy(ctxt->sax, sax, sizeof(htmlSAXHandler));
	if (user_data != NULL)
	    ctxt->userData = user_data;
    }
    if (filename == NULL) {
	ctxt->directory = NULL;
    } else {
        ctxt->directory = xmlParserGetDirectory(filename);
    }

    inputStream = htmlNewInputStream(ctxt);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(ctxt);
	xmlFree(buf);
	return(NULL);
    }

    if (filename == NULL)
	inputStream->filename = NULL;
    else
	inputStream->filename = (char *)
	    xmlCanonicPath((const xmlChar *) filename);
    inputStream->buf = buf;
    xmlBufResetInput(buf->buffer, inputStream);

    inputPush(ctxt, inputStream);

    if ((size > 0) && (chunk != NULL) && (ctxt->input != NULL) &&
        (ctxt->input->buf != NULL))  {
	size_t base = xmlBufGetInputBase(ctxt->input->buf->buffer, ctxt->input);
	size_t cur = ctxt->input->cur - ctxt->input->base;

	xmlParserInputBufferPush(ctxt->input->buf, size, chunk);

        xmlBufSetInputBaseCur(ctxt->input->buf->buffer, ctxt->input, base, cur);
#ifdef DEBUG_PUSH
	xmlGenericError(xmlGenericErrorContext, "HPP: pushed %d\n", size);
#endif
    }
    ctxt->progressive = 1;

    return(ctxt);
}