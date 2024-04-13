xmlSaveFlush(xmlSaveCtxtPtr ctxt)
{
    if (ctxt == NULL) return(-1);
    if (ctxt->buf == NULL) return(-1);
    return(xmlOutputBufferFlush(ctxt->buf));
}