xmlNewStringInputStream(xmlParserCtxtPtr ctxt, const xmlChar *buffer) {
    xmlParserInputPtr input;

    if (buffer == NULL) {
        xmlErrInternal(ctxt, "xmlNewStringInputStream string = NULL\n",
	               NULL);
	return(NULL);
    }
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"new fixed input: %.30s\n", buffer);
    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
        xmlErrMemory(ctxt,  "couldn't allocate a new input stream\n");
	return(NULL);
    }
    input->base = buffer;
    input->cur = buffer;
    input->length = xmlStrlen(buffer);
    input->end = &buffer[input->length];
    return(input);
}