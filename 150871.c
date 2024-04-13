xmlValidatePushCData(xmlValidCtxtPtr ctxt, const xmlChar *data, int len) {
    int ret = 1;

/* printf("CDATA %s %d\n", data, len); */
    if (ctxt == NULL)
        return(0);
    if (len <= 0)
	return(ret);
    if ((ctxt->vstateNr > 0) && (ctxt->vstate != NULL)) {
	xmlValidStatePtr state = ctxt->vstate;
	xmlElementPtr elemDecl;

	/*
	 * Check the new element agaisnt the content model of the new elem.
	 */
	if (state->elemDecl != NULL) {
	    elemDecl = state->elemDecl;

	    switch(elemDecl->etype) {
		case XML_ELEMENT_TYPE_UNDEFINED:
		    ret = 0;
		    break;
		case XML_ELEMENT_TYPE_EMPTY:
		    xmlErrValidNode(ctxt, state->node,
				    XML_DTD_NOT_EMPTY,
	       "Element %s was declared EMPTY this one has content\n",
			   state->node->name, NULL, NULL);
		    ret = 0;
		    break;
		case XML_ELEMENT_TYPE_ANY:
		    break;
		case XML_ELEMENT_TYPE_MIXED:
		    break;
		case XML_ELEMENT_TYPE_ELEMENT:
		    if (len > 0) {
			int i;

			for (i = 0;i < len;i++) {
			    if (!IS_BLANK_CH(data[i])) {
				xmlErrValidNode(ctxt, state->node,
						XML_DTD_CONTENT_MODEL,
	   "Element %s content does not follow the DTD, Text not allowed\n",
				       state->node->name, NULL, NULL);
				ret = 0;
				goto done;
			    }
			}
			/*
			 * TODO:
			 * VC: Standalone Document Declaration
			 *  element types with element content, if white space
			 *  occurs directly within any instance of those types.
			 */
		    }
		    break;
	    }
	}
    }
done:
    return(ret);
}