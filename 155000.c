xmlNewIOInputStream(xmlParserCtxtPtr ctxt, xmlParserInputBufferPtr input,
	            xmlCharEncoding enc) {
    xmlParserInputPtr inputStream;

    if (input == NULL) return(NULL);
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext, "new input from I/O\n");
    inputStream = xmlNewInputStream(ctxt);
    if (inputStream == NULL) {
	return(NULL);
    }
    inputStream->filename = NULL;
    inputStream->buf = input;
    xmlBufResetInput(inputStream->buf->buffer, inputStream);

    if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }

    return(inputStream);
}