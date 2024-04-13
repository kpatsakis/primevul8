xmlSaveClose(xmlSaveCtxtPtr ctxt)
{
    int ret;

    if (ctxt == NULL) return(-1);
    ret = xmlSaveFlush(ctxt);
    xmlFreeSaveCtxt(ctxt);
    return(ret);
}