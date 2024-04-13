xmlNodeListDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur) {
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    while (cur != NULL) {
	if ((ctxt->format == 1) && (xmlIndentTreeOutput) &&
	    ((cur->type == XML_ELEMENT_NODE) ||
	     (cur->type == XML_COMMENT_NODE) ||
	     (cur->type == XML_PI_NODE)))
	    xmlOutputBufferWrite(buf, ctxt->indent_size *
	                         (ctxt->level > ctxt->indent_nr ?
				  ctxt->indent_nr : ctxt->level),
				 ctxt->indent);
        xmlNodeDumpOutputInternal(ctxt, cur);
	if (ctxt->format == 1) {
	    xmlOutputBufferWrite(buf, 1, "\n");
	}
	cur = cur->next;
    }
}