xmlOutputBufferWriteWSNonSig(xmlSaveCtxtPtr ctxt, int extra)
{
    int i;
    if ((ctxt == NULL) || (ctxt->buf == NULL))
        return;
    xmlOutputBufferWrite(ctxt->buf, 1, "\n");
    for (i = 0; i < (ctxt->level + extra); i += ctxt->indent_nr) {
        xmlOutputBufferWrite(ctxt->buf, ctxt->indent_size *
                ((ctxt->level + extra - i) > ctxt->indent_nr ?
                 ctxt->indent_nr : (ctxt->level + extra - i)),
                ctxt->indent);
    }
}