xmlValidatePushElement(xmlValidCtxtPtr ctxt, xmlDocPtr doc,
                       xmlNodePtr elem, const xmlChar *qname) {
    int ret = 1;
    xmlElementPtr eDecl;
    int extsubset = 0;

    if (ctxt == NULL)
        return(0);
/* printf("PushElem %s\n", qname); */
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
		    /* I don't think anything is required then */
		    break;
		case XML_ELEMENT_TYPE_MIXED:
		    /* simple case of declared as #PCDATA */
		    if ((elemDecl->content != NULL) &&
			(elemDecl->content->type ==
			 XML_ELEMENT_CONTENT_PCDATA)) {
			xmlErrValidNode(ctxt, state->node,
					XML_DTD_NOT_PCDATA,
	       "Element %s was declared #PCDATA but contains non text nodes\n",
				state->node->name, NULL, NULL);
			ret = 0;
		    } else {
			ret = xmlValidateCheckMixed(ctxt, elemDecl->content,
				                    qname);
			if (ret != 1) {
			    xmlErrValidNode(ctxt, state->node,
					    XML_DTD_INVALID_CHILD,
	       "Element %s is not declared in %s list of possible children\n",
				    qname, state->node->name, NULL);
			}
		    }
		    break;
		case XML_ELEMENT_TYPE_ELEMENT:
		    /*
		     * TODO:
		     * VC: Standalone Document Declaration
		     *     - element types with element content, if white space
		     *       occurs directly within any instance of those types.
		     */
		    if (state->exec != NULL) {
			ret = xmlRegExecPushString(state->exec, qname, NULL);
			if (ret < 0) {
			    xmlErrValidNode(ctxt, state->node,
					    XML_DTD_CONTENT_MODEL,
	       "Element %s content does not follow the DTD, Misplaced %s\n",
				   state->node->name, qname, NULL);
			    ret = 0;
			} else {
			    ret = 1;
			}
		    }
		    break;
	    }
	}
    }
    eDecl = xmlValidGetElemDecl(ctxt, doc, elem, &extsubset);
    vstateVPush(ctxt, eDecl, elem);
    return(ret);
}