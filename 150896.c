xmlValidatePopElement(xmlValidCtxtPtr ctxt, xmlDocPtr doc ATTRIBUTE_UNUSED,
                      xmlNodePtr elem ATTRIBUTE_UNUSED,
		      const xmlChar *qname ATTRIBUTE_UNUSED) {
    int ret = 1;

    if (ctxt == NULL)
        return(0);
/* printf("PopElem %s\n", qname); */
    if ((ctxt->vstateNr > 0) && (ctxt->vstate != NULL)) {
	xmlValidStatePtr state = ctxt->vstate;
	xmlElementPtr elemDecl;

	/*
	 * Check the new element agaisnt the content model of the new elem.
	 */
	if (state->elemDecl != NULL) {
	    elemDecl = state->elemDecl;

	    if (elemDecl->etype == XML_ELEMENT_TYPE_ELEMENT) {
		if (state->exec != NULL) {
		    ret = xmlRegExecPushString(state->exec, NULL, NULL);
		    if (ret == 0) {
			xmlErrValidNode(ctxt, state->node,
			                XML_DTD_CONTENT_MODEL,
	   "Element %s content does not follow the DTD, Expecting more child\n",
			       state->node->name, NULL,NULL);
		    } else {
			/*
			 * previous validation errors should not generate
			 * a new one here
			 */
			ret = 1;
		    }
		}
	    }
	}
	vstateVPop(ctxt);
    }
    return(ret);
}