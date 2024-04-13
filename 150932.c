xmlErrValidWarning(xmlValidCtxtPtr ctxt,
                xmlNodePtr node, xmlParserErrors error,
                const char *msg, const xmlChar * str1,
                const xmlChar * str2, const xmlChar * str3)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    xmlParserCtxtPtr pctxt = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->warning;
        data = ctxt->userData;
	/* Use the special values to detect if it is part of a parsing
	   context */
	if ((ctxt->finishDtd == XML_CTXT_FINISH_DTD_0) ||
	    (ctxt->finishDtd == XML_CTXT_FINISH_DTD_1)) {
	    long delta = (char *) ctxt - (char *) ctxt->userData;
	    if ((delta > 0) && (delta < 250))
		pctxt = ctxt->userData;
	}
    }
    __xmlRaiseError(schannel, channel, data, pctxt, node, XML_FROM_VALID, error,
                    XML_ERR_WARNING, NULL, 0,
                    (const char *) str1,
                    (const char *) str1,
                    (const char *) str3, 0, 0, msg, str1, str2, str3);
}