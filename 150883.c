xmlValidPrintNodeList(xmlNodePtr cur) {
    if (cur == NULL)
	xmlGenericError(xmlGenericErrorContext, "null ");
    while (cur != NULL) {
	xmlValidPrintNode(cur);
	cur = cur->next;
    }
}