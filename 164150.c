htmlCreateMemoryParserCtxt(const char *buffer, int size) {
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr input;
    xmlParserInputBufferPtr buf;

    if (buffer == NULL)
	return(NULL);
    if (size <= 0)
	return(NULL);

    ctxt = htmlNewParserCtxt();
    if (ctxt == NULL)
	return(NULL);

    buf = xmlParserInputBufferCreateMem(buffer, size, XML_CHAR_ENCODING_NONE);
    if (buf == NULL) return(NULL);

    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    input->filename = NULL;
    input->buf = buf;
    xmlBufResetInput(buf->buffer, input);

    inputPush(ctxt, input);
    return(ctxt);
}