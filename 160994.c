xmlDtdDumpOutput(xmlSaveCtxtPtr ctxt, xmlDtdPtr dtd) {
    xmlOutputBufferPtr buf;
    int format, level;
    xmlDocPtr doc;

    if (dtd == NULL) return;
    if ((ctxt == NULL) || (ctxt->buf == NULL))
        return;
    buf = ctxt->buf;
    xmlOutputBufferWrite(buf, 10, "<!DOCTYPE ");
    xmlOutputBufferWriteString(buf, (const char *)dtd->name);
    if (dtd->ExternalID != NULL) {
	xmlOutputBufferWrite(buf, 8, " PUBLIC ");
	xmlBufWriteQuotedString(buf->buffer, dtd->ExternalID);
	xmlOutputBufferWrite(buf, 1, " ");
	xmlBufWriteQuotedString(buf->buffer, dtd->SystemID);
    }  else if (dtd->SystemID != NULL) {
	xmlOutputBufferWrite(buf, 8, " SYSTEM ");
	xmlBufWriteQuotedString(buf->buffer, dtd->SystemID);
    }
    if ((dtd->entities == NULL) && (dtd->elements == NULL) &&
        (dtd->attributes == NULL) && (dtd->notations == NULL) &&
	(dtd->pentities == NULL)) {
	xmlOutputBufferWrite(buf, 1, ">");
	return;
    }
    xmlOutputBufferWrite(buf, 3, " [\n");
    /*
     * Dump the notations first they are not in the DTD children list
     * Do this only on a standalone DTD or on the internal subset though.
     */
    if ((dtd->notations != NULL) && ((dtd->doc == NULL) ||
        (dtd->doc->intSubset == dtd))) {
        xmlBufDumpNotationTable(buf->buffer,
                                (xmlNotationTablePtr) dtd->notations);
    }
    format = ctxt->format;
    level = ctxt->level;
    doc = ctxt->doc;
    ctxt->format = 0;
    ctxt->level = -1;
    ctxt->doc = dtd->doc;
    xmlNodeListDumpOutput(ctxt, dtd->children);
    ctxt->format = format;
    ctxt->level = level;
    ctxt->doc = doc;
    xmlOutputBufferWrite(buf, 2, "]>");
}