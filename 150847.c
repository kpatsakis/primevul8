nodeVPush(xmlValidCtxtPtr ctxt, xmlNodePtr value)
{
    if (ctxt->nodeMax <= 0) {
        ctxt->nodeMax = 4;
        ctxt->nodeTab =
            (xmlNodePtr *) xmlMalloc(ctxt->nodeMax *
                                     sizeof(ctxt->nodeTab[0]));
        if (ctxt->nodeTab == NULL) {
	    xmlVErrMemory(ctxt, "malloc failed");
            ctxt->nodeMax = 0;
            return (0);
        }
    }
    if (ctxt->nodeNr >= ctxt->nodeMax) {
        xmlNodePtr *tmp;
        tmp = (xmlNodePtr *) xmlRealloc(ctxt->nodeTab,
			      ctxt->nodeMax * 2 * sizeof(ctxt->nodeTab[0]));
        if (tmp == NULL) {
	    xmlVErrMemory(ctxt, "realloc failed");
            return (0);
        }
        ctxt->nodeMax *= 2;
	ctxt->nodeTab = tmp;
    }
    ctxt->nodeTab[ctxt->nodeNr] = value;
    ctxt->node = value;
    return (ctxt->nodeNr++);
}