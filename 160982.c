xmlSaveFormatFileEnc( const char * filename, xmlDocPtr cur,
			const char * encoding, int format ) {
    xmlSaveCtxt ctxt;
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    int ret;

    if (cur == NULL)
	return(-1);

    if (encoding == NULL)
	encoding = (const char *) cur->encoding;

    if (encoding != NULL) {

	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL)
		return(-1);
    }

#ifdef HAVE_ZLIB_H
    if (cur->compression < 0) cur->compression = xmlGetCompressMode();
#endif
    /*
     * save the content to a temp buffer.
     */
    buf = xmlOutputBufferCreateFilename(filename, handler, cur->compression);
    if (buf == NULL) return(-1);
    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = cur;
    ctxt.buf = buf;
    ctxt.level = 0;
    ctxt.format = format ? 1 : 0;
    ctxt.encoding = (const xmlChar *) encoding;
    xmlSaveCtxtInit(&ctxt);
    ctxt.options |= XML_SAVE_AS_XML;

    xmlDocContentDumpOutput(&ctxt, cur);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}