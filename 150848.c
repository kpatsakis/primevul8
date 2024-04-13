xmlValidBuildContentModel(xmlValidCtxtPtr ctxt, xmlElementPtr elem) {

    if ((ctxt == NULL) || (elem == NULL))
	return(0);
    if (elem->type != XML_ELEMENT_DECL)
	return(0);
    if (elem->etype != XML_ELEMENT_TYPE_ELEMENT)
	return(1);
    /* TODO: should we rebuild in this case ? */
    if (elem->contModel != NULL) {
	if (!xmlRegexpIsDeterminist(elem->contModel)) {
	    ctxt->valid = 0;
	    return(0);
	}
	return(1);
    }

    ctxt->am = xmlNewAutomata();
    if (ctxt->am == NULL) {
	xmlErrValidNode(ctxt, (xmlNodePtr) elem,
	                XML_ERR_INTERNAL_ERROR,
	                "Cannot create automata for element %s\n",
		        elem->name, NULL, NULL);
	return(0);
    }
    ctxt->state = xmlAutomataGetInitState(ctxt->am);
    xmlValidBuildAContentModel(elem->content, ctxt, elem->name);
    xmlAutomataSetFinalState(ctxt->am, ctxt->state);
    elem->contModel = xmlAutomataCompile(ctxt->am);
    if (xmlRegexpIsDeterminist(elem->contModel) != 1) {
	char expr[5000];
	expr[0] = 0;
	xmlSnprintfElementContent(expr, 5000, elem->content, 1);
	xmlErrValidNode(ctxt, (xmlNodePtr) elem,
	                XML_DTD_CONTENT_NOT_DETERMINIST,
	       "Content model of %s is not determinist: %s\n",
	       elem->name, BAD_CAST expr, NULL);
#ifdef DEBUG_REGEXP_ALGO
        xmlRegexpPrint(stderr, elem->contModel);
#endif
        ctxt->valid = 0;
	ctxt->state = NULL;
	xmlFreeAutomata(ctxt->am);
	ctxt->am = NULL;
	return(0);
    }
    ctxt->state = NULL;
    xmlFreeAutomata(ctxt->am);
    ctxt->am = NULL;
    return(1);
}