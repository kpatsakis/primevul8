htmlNodeInfoPush(htmlParserCtxtPtr ctxt, htmlParserNodeInfo *value)
{
    if (ctxt->nodeInfoNr >= ctxt->nodeInfoMax) {
        if (ctxt->nodeInfoMax == 0)
                ctxt->nodeInfoMax = 5;
        ctxt->nodeInfoMax *= 2;
        ctxt->nodeInfoTab = (htmlParserNodeInfo *)
                         xmlRealloc((htmlParserNodeInfo *)ctxt->nodeInfoTab,
                                    ctxt->nodeInfoMax *
                                    sizeof(ctxt->nodeInfoTab[0]));
        if (ctxt->nodeInfoTab == NULL) {
            htmlErrMemory(ctxt, NULL);
            return (0);
        }
    }
    ctxt->nodeInfoTab[ctxt->nodeInfoNr] = *value;
    ctxt->nodeInfo = &ctxt->nodeInfoTab[ctxt->nodeInfoNr];
    return (ctxt->nodeInfoNr++);
}