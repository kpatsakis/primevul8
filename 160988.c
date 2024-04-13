xmlSaveToFd(int fd, const char *encoding, int options)
{
    xmlSaveCtxtPtr ret;

    ret = xmlNewSaveCtxt(encoding, options);
    if (ret == NULL) return(NULL);
    ret->buf = xmlOutputBufferCreateFd(fd, ret->handler);
    if (ret->buf == NULL) {
	xmlFreeSaveCtxt(ret);
	return(NULL);
    }
    return(ret);
}