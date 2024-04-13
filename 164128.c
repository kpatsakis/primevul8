htmlAutoClose(htmlParserCtxtPtr ctxt, const xmlChar * newtag)
{
    while ((newtag != NULL) && (ctxt->name != NULL) &&
           (htmlCheckAutoClose(newtag, ctxt->name))) {
        if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
            ctxt->sax->endElement(ctxt->userData, ctxt->name);
	htmlnamePop(ctxt);
    }
    if (newtag == NULL) {
        htmlAutoCloseOnEnd(ctxt);
        return;
    }
    while ((newtag == NULL) && (ctxt->name != NULL) &&
           ((xmlStrEqual(ctxt->name, BAD_CAST "head")) ||
            (xmlStrEqual(ctxt->name, BAD_CAST "body")) ||
            (xmlStrEqual(ctxt->name, BAD_CAST "html")))) {
        if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
            ctxt->sax->endElement(ctxt->userData, ctxt->name);
	htmlnamePop(ctxt);
    }
}