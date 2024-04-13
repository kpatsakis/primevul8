xmlErrValid(xmlValidCtxtPtr ctxt, xmlParserErrors error,
            const char *msg, const char *extra)
{
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
    if (extra)
        __xmlRaiseError(NULL, channel, data,
                        pctxt, NULL, XML_FROM_VALID, error,
                        XML_ERR_ERROR, NULL, 0, extra, NULL, NULL, 0, 0,
                        msg, extra);
    else
        __xmlRaiseError(NULL, channel, data,
                        pctxt, NULL, XML_FROM_VALID, error,
                        XML_ERR_ERROR, NULL, 0, NULL, NULL, NULL, 0, 0,
                        "%s", msg);
}