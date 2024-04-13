xmlErrInternal(xmlParserCtxtPtr ctxt, const char *msg, const xmlChar * str)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL)
        ctxt->errNo = XML_ERR_INTERNAL_ERROR;
    __xmlRaiseError(NULL, NULL, NULL,
                    ctxt, NULL, XML_FROM_PARSER, XML_ERR_INTERNAL_ERROR,
                    XML_ERR_FATAL, NULL, 0, (const char *) str, NULL, NULL,
                    0, 0, msg, str);
    if (ctxt != NULL) {
        ctxt->wellFormed = 0;
        if (ctxt->recovery == 0)
            ctxt->disableSAX = 1;
    }
}