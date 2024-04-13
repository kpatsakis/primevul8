xmlSaveToIO(xmlOutputWriteCallback iowrite,
            xmlOutputCloseCallback ioclose,
            void *ioctx, const char *encoding, int options)
{
    xmlSaveCtxtPtr ret;

    ret = xmlNewSaveCtxt(encoding, options);
    if (ret == NULL) return(NULL);
    ret->buf = xmlOutputBufferCreateIO(iowrite, ioclose, ioctx, ret->handler);
    if (ret->buf == NULL) {
	xmlFreeSaveCtxt(ret);
	return(NULL);
    }
    return(ret);
}