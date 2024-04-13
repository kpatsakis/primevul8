xmlValidateSkipIgnorable(xmlNodePtr child) {
    while (child != NULL) {
	switch (child->type) {
	    /* These things are ignored (skipped) during validation.  */
	    case XML_PI_NODE:
	    case XML_COMMENT_NODE:
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
		child = child->next;
		break;
	    case XML_TEXT_NODE:
		if (xmlIsBlankNode(child))
		    child = child->next;
		else
		    return(child);
		break;
	    /* keep current node */
	    default:
		return(child);
	}
    }
    return(child);
}