htmlParseErr(xmlParserCtxtPtr ctxt, xmlParserErrors error,
             const char *msg, const xmlChar *str1, const xmlChar *str2)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL)
	ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_HTML, error,
                    XML_ERR_ERROR, NULL, 0,
		    (const char *) str1, (const char *) str2,
		    NULL, 0, 0,
		    msg, str1, str2);
    if (ctxt != NULL)
	ctxt->wellFormed = 0;
}