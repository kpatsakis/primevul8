htmlErrMemory(xmlParserCtxtPtr ctxt, const char *extra)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL) {
        ctxt->errNo = XML_ERR_NO_MEMORY;
        ctxt->instate = XML_PARSER_EOF;
        ctxt->disableSAX = 1;
    }
    if (extra)
        __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, extra,
                        NULL, NULL, 0, 0,
                        "Memory allocation failed : %s\n", extra);
    else
        __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, NULL,
                        NULL, NULL, 0, 0, "Memory allocation failed\n");
}