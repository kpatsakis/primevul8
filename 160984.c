xmlSaveSetAttrEscape(xmlSaveCtxtPtr ctxt, xmlCharEncodingOutputFunc escape)
{
    if (ctxt == NULL) return(-1);
    ctxt->escapeAttr = escape;
    return(0);
}