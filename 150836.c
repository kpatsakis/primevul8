nodeVPop(xmlValidCtxtPtr ctxt)
{
    xmlNodePtr ret;

    if (ctxt->nodeNr <= 0)
        return (NULL);
    ctxt->nodeNr--;
    if (ctxt->nodeNr > 0)
        ctxt->node = ctxt->nodeTab[ctxt->nodeNr - 1];
    else
        ctxt->node = NULL;
    ret = ctxt->nodeTab[ctxt->nodeNr];
    ctxt->nodeTab[ctxt->nodeNr] = NULL;
    return (ret);
}