htmlParseErrInt(xmlParserCtxtPtr ctxt, xmlParserErrors error,
             const char *msg, int val)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL)
	ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_HTML, error,
                    XML_ERR_ERROR, NULL, 0, NULL, NULL,
		    NULL, val, 0, msg, val);
    if (ctxt != NULL)
	ctxt->wellFormed = 0;
}