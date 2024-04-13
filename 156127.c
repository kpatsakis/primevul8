xmlParseElement(xmlParserCtxtPtr ctxt) {
    if (xmlParseElementStart(ctxt) != 0)
        return;
    xmlParseContent(ctxt);
    if (ctxt->instate == XML_PARSER_EOF)
	return;
    xmlParseElementEnd(ctxt);
}