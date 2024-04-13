htmlAutoCloseOnEnd(htmlParserCtxtPtr ctxt)
{
    int i;

    if (ctxt->nameNr == 0)
        return;
    for (i = (ctxt->nameNr - 1); i >= 0; i--) {
        if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
            ctxt->sax->endElement(ctxt->userData, ctxt->name);
	htmlnamePop(ctxt);
    }
}