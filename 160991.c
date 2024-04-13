xmlSaveToBuffer(xmlBufferPtr buffer, const char *encoding, int options)
{
    xmlSaveCtxtPtr ret;
    xmlOutputBufferPtr out_buff;
    xmlCharEncodingHandlerPtr handler;

    ret = xmlNewSaveCtxt(encoding, options);
    if (ret == NULL) return(NULL);

    if (encoding != NULL) {
        handler = xmlFindCharEncodingHandler(encoding);
        if (handler == NULL) {
            xmlFree(ret);
            return(NULL);
        }
    } else
        handler = NULL;
    out_buff = xmlOutputBufferCreateBuffer(buffer, handler);
    if (out_buff == NULL) {
        xmlFree(ret);
        if (handler) xmlCharEncCloseFunc(handler);
        return(NULL);
    }

    ret->buf = out_buff;
    return(ret);
}