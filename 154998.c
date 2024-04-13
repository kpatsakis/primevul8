xmlNewInputStream(xmlParserCtxtPtr ctxt) {
    xmlParserInputPtr input;

    input = (xmlParserInputPtr) xmlMalloc(sizeof(xmlParserInput));
    if (input == NULL) {
        xmlErrMemory(ctxt,  "couldn't allocate a new input stream\n");
	return(NULL);
    }
    memset(input, 0, sizeof(xmlParserInput));
    input->line = 1;
    input->col = 1;
    input->standalone = -1;

    /*
     * If the context is NULL the id cannot be initialized, but that
     * should not happen while parsing which is the situation where
     * the id is actually needed.
     */
    if (ctxt != NULL)
        input->id = ctxt->input_id++;

    return(input);
}