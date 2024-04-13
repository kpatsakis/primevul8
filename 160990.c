xmlBufNodeDump(xmlBufPtr buf, xmlDocPtr doc, xmlNodePtr cur, int level,
            int format)
{
    size_t use;
    int ret;
    xmlOutputBufferPtr outbuf;
    int oldalloc;

    xmlInitParser();

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                        "xmlNodeDump : node == NULL\n");
#endif
        return (-1);
    }
    if (buf == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                        "xmlNodeDump : buf == NULL\n");
#endif
        return (-1);
    }
    outbuf = (xmlOutputBufferPtr) xmlMalloc(sizeof(xmlOutputBuffer));
    if (outbuf == NULL) {
        xmlSaveErrMemory("creating buffer");
        return (-1);
    }
    memset(outbuf, 0, (size_t) sizeof(xmlOutputBuffer));
    outbuf->buffer = buf;
    outbuf->encoder = NULL;
    outbuf->writecallback = NULL;
    outbuf->closecallback = NULL;
    outbuf->context = NULL;
    outbuf->written = 0;

    use = xmlBufUse(buf);
    oldalloc = xmlBufGetAllocationScheme(buf);
    xmlBufSetAllocationScheme(buf, XML_BUFFER_ALLOC_DOUBLEIT);
    xmlNodeDumpOutput(outbuf, doc, cur, level, format, NULL);
    xmlBufSetAllocationScheme(buf, oldalloc);
    xmlFree(outbuf);
    ret = xmlBufUse(buf) - use;
    return (ret);
}