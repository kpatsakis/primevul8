xmlNsDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur, xmlSaveCtxtPtr ctxt) {
    if ((cur == NULL) || (buf == NULL)) return;
    if ((cur->type == XML_LOCAL_NAMESPACE) && (cur->href != NULL)) {
	if (xmlStrEqual(cur->prefix, BAD_CAST "xml"))
	    return;

	if (ctxt != NULL && ctxt->format == 2)
	    xmlOutputBufferWriteWSNonSig(ctxt, 2);
	else
	    xmlOutputBufferWrite(buf, 1, " ");

        /* Within the context of an element attributes */
	if (cur->prefix != NULL) {
	    xmlOutputBufferWrite(buf, 6, "xmlns:");
	    xmlOutputBufferWriteString(buf, (const char *)cur->prefix);
	} else
	    xmlOutputBufferWrite(buf, 5, "xmlns");
	xmlOutputBufferWrite(buf, 1, "=");
	xmlBufWriteQuotedString(buf->buffer, cur->href);
    }
}