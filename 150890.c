xmlValidateOneCdataElement(xmlValidCtxtPtr ctxt, xmlDocPtr doc,
                           xmlNodePtr elem) {
    int ret = 1;
    xmlNodePtr cur, child;

    if ((ctxt == NULL) || (doc == NULL) || (elem == NULL) ||
        (elem->type != XML_ELEMENT_NODE))
	return(0);

    child = elem->children;

    cur = child;
    while (cur != NULL) {
	switch (cur->type) {
	    case XML_ENTITY_REF_NODE:
		/*
		 * Push the current node to be able to roll back
		 * and process within the entity
		 */
		if ((cur->children != NULL) &&
		    (cur->children->children != NULL)) {
		    nodeVPush(ctxt, cur);
		    cur = cur->children->children;
		    continue;
		}
		break;
	    case XML_COMMENT_NODE:
	    case XML_PI_NODE:
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
		break;
	    default:
		ret = 0;
		goto done;
	}
	/*
	 * Switch to next element
	 */
	cur = cur->next;
	while (cur == NULL) {
	    cur = nodeVPop(ctxt);
	    if (cur == NULL)
		break;
	    cur = cur->next;
	}
    }
done:
    ctxt->nodeMax = 0;
    ctxt->nodeNr = 0;
    if (ctxt->nodeTab != NULL) {
	xmlFree(ctxt->nodeTab);
	ctxt->nodeTab = NULL;
    }
    return(ret);
}