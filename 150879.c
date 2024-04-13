xmlErrValidNodeNr(xmlValidCtxtPtr ctxt,
                xmlNodePtr node, xmlParserErrors error,
                const char *msg, const xmlChar * str1,
                int int2, const xmlChar * str3)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    xmlParserCtxtPtr pctxt = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
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
                    XML_ERR_ERROR, NULL, 0,
                    (const char *) str1,
                    (const char *) str3,
                    NULL, int2, 0, msg, str1, int2, str3);
}