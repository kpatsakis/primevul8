xmlNsListDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur) {
    while (cur != NULL) {
        xmlNsDumpOutput(buf, cur, NULL);
	cur = cur->next;
    }
}