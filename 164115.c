htmlCtxtUseOptions(htmlParserCtxtPtr ctxt, int options)
{
    if (ctxt == NULL)
        return(-1);

    if (options & HTML_PARSE_NOWARNING) {
        ctxt->sax->warning = NULL;
        ctxt->vctxt.warning = NULL;
        options -= XML_PARSE_NOWARNING;
	ctxt->options |= XML_PARSE_NOWARNING;
    }
    if (options & HTML_PARSE_NOERROR) {
        ctxt->sax->error = NULL;
        ctxt->vctxt.error = NULL;
        ctxt->sax->fatalError = NULL;
        options -= XML_PARSE_NOERROR;
	ctxt->options |= XML_PARSE_NOERROR;
    }
    if (options & HTML_PARSE_PEDANTIC) {
        ctxt->pedantic = 1;
        options -= XML_PARSE_PEDANTIC;
	ctxt->options |= XML_PARSE_PEDANTIC;
    } else
        ctxt->pedantic = 0;
    if (options & XML_PARSE_NOBLANKS) {
        ctxt->keepBlanks = 0;
        ctxt->sax->ignorableWhitespace = xmlSAX2IgnorableWhitespace;
        options -= XML_PARSE_NOBLANKS;
	ctxt->options |= XML_PARSE_NOBLANKS;
    } else
        ctxt->keepBlanks = 1;
    if (options & HTML_PARSE_RECOVER) {
        ctxt->recovery = 1;
	options -= HTML_PARSE_RECOVER;
    } else
        ctxt->recovery = 0;
    if (options & HTML_PARSE_COMPACT) {
	ctxt->options |= HTML_PARSE_COMPACT;
        options -= HTML_PARSE_COMPACT;
    }
    if (options & XML_PARSE_HUGE) {
	ctxt->options |= XML_PARSE_HUGE;
        options -= XML_PARSE_HUGE;
    }
    if (options & HTML_PARSE_NODEFDTD) {
	ctxt->options |= HTML_PARSE_NODEFDTD;
        options -= HTML_PARSE_NODEFDTD;
    }
    if (options & HTML_PARSE_IGNORE_ENC) {
	ctxt->options |= HTML_PARSE_IGNORE_ENC;
        options -= HTML_PARSE_IGNORE_ENC;
    }
    if (options & HTML_PARSE_NOIMPLIED) {
        ctxt->options |= HTML_PARSE_NOIMPLIED;
        options -= HTML_PARSE_NOIMPLIED;
    }
    ctxt->dictNames = 0;
    return (options);
}