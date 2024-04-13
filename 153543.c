loadSomething(URLFile *f,
	      Buffer *(*loadproc) (URLFile *, Buffer *), Buffer *defaultbuf)
{
    Buffer *buf;

    if ((buf = loadproc(f, defaultbuf)) == NULL)
	return NULL;

    if (buf->buffername == NULL || buf->buffername[0] == '\0') {
	buf->buffername = checkHeader(buf, "Subject:");
	if (buf->buffername == NULL && buf->filename != NULL)
	    buf->buffername = conv_from_system(lastFileName(buf->filename));
    }
    if (buf->currentURL.scheme == SCM_UNKNOWN)
	buf->currentURL.scheme = f->scheme;
    if (f->scheme == SCM_LOCAL && buf->sourcefile == NULL)
	buf->sourcefile = buf->filename;
    if (loadproc == loadHTMLBuffer
#ifdef USE_IMAGE
	|| loadproc == loadImageBuffer
#endif
       )
	buf->type = "text/html";
    else
	buf->type = "text/plain";
    return buf;
}