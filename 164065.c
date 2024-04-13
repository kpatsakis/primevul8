__htmlParseContent(void *ctxt) {
    if (ctxt != NULL)
	htmlParseContentInternal((htmlParserCtxtPtr) ctxt);
}