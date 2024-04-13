xmlSaveDoc(xmlSaveCtxtPtr ctxt, xmlDocPtr doc)
{
    long ret = 0;

    if ((ctxt == NULL) || (doc == NULL)) return(-1);
    if (xmlDocContentDumpOutput(ctxt, doc) < 0)
        return(-1);
    return(ret);
}