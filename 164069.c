htmlnamePush(htmlParserCtxtPtr ctxt, const xmlChar * value)
{
    if ((ctxt->html < 3) && (xmlStrEqual(value, BAD_CAST "head")))
        ctxt->html = 3;
    if ((ctxt->html < 10) && (xmlStrEqual(value, BAD_CAST "body")))
        ctxt->html = 10;
    if (ctxt->nameNr >= ctxt->nameMax) {
        ctxt->nameMax *= 2;
        ctxt->nameTab = (const xmlChar * *)
                         xmlRealloc((xmlChar * *)ctxt->nameTab,
                                    ctxt->nameMax *
                                    sizeof(ctxt->nameTab[0]));
        if (ctxt->nameTab == NULL) {
            htmlErrMemory(ctxt, NULL);
            return (0);
        }
    }
    ctxt->nameTab[ctxt->nameNr] = value;
    ctxt->name = value;
    return (ctxt->nameNr++);
}