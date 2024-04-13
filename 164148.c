htmlNewInputStream(htmlParserCtxtPtr ctxt) {
    htmlParserInputPtr input;

    input = (xmlParserInputPtr) xmlMalloc(sizeof(htmlParserInput));
    if (input == NULL) {
        htmlErrMemory(ctxt, "couldn't allocate a new input stream\n");
	return(NULL);
    }
    memset(input, 0, sizeof(htmlParserInput));
    input->filename = NULL;
    input->directory = NULL;
    input->base = NULL;
    input->cur = NULL;
    input->buf = NULL;
    input->line = 1;
    input->col = 1;
    input->buf = NULL;
    input->free = NULL;
    input->version = NULL;
    input->consumed = 0;
    input->length = 0;
    return(input);
}