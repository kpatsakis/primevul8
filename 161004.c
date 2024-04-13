xmlAttrListDumpOutput(xmlSaveCtxtPtr ctxt, xmlAttrPtr cur) {
    if (cur == NULL) return;
    while (cur != NULL) {
        xmlAttrDumpOutput(ctxt, cur);
	cur = cur->next;
    }
}