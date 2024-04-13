xmlSaveToFilename(const char *filename, const char *encoding, int options)
{
    xmlSaveCtxtPtr ret;
    int compression = 0; /* TODO handle compression option */

    ret = xmlNewSaveCtxt(encoding, options);
    if (ret == NULL) return(NULL);
    ret->buf = xmlOutputBufferCreateFilename(filename, ret->handler,
                                             compression);
    if (ret->buf == NULL) {
	xmlFreeSaveCtxt(ret);
	return(NULL);
    }
    return(ret);
}