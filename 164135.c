htmlReadMemory(const char *buffer, int size, const char *URL, const char *encoding, int options)
{
    htmlParserCtxtPtr ctxt;

    xmlInitParser();
    ctxt = xmlCreateMemoryParserCtxt(buffer, size);
    if (ctxt == NULL)
        return (NULL);
    htmlDefaultSAXHandlerInit();
    if (ctxt->sax != NULL)
        memcpy(ctxt->sax, &htmlDefaultSAXHandler, sizeof(xmlSAXHandlerV1));
    return (htmlDoRead(ctxt, URL, encoding, options, 0));
}