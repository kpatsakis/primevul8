xmlSaveTree(xmlSaveCtxtPtr ctxt, xmlNodePtr node)
{
    long ret = 0;

    if ((ctxt == NULL) || (node == NULL)) return(-1);
    xmlNodeDumpOutputInternal(ctxt, node);
    return(ret);
}