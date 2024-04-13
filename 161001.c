xmlNsListDumpOutputCtxt(xmlSaveCtxtPtr ctxt, xmlNsPtr cur) {
    while (cur != NULL) {
        xmlNsDumpOutput(ctxt->buf, cur, ctxt);
	cur = cur->next;
    }
}