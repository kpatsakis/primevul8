vstateVPush(xmlValidCtxtPtr ctxt, xmlElementPtr elemDecl, xmlNodePtr node) {
    if ((ctxt->vstateMax == 0) || (ctxt->vstateTab == NULL)) {
	ctxt->vstateMax = 10;
	ctxt->vstateTab = (xmlValidState *) xmlMalloc(ctxt->vstateMax *
		              sizeof(ctxt->vstateTab[0]));
        if (ctxt->vstateTab == NULL) {
	    xmlVErrMemory(ctxt, "malloc failed");
	    return(-1);
	}
    }

    if (ctxt->vstateNr >= ctxt->vstateMax) {
        xmlValidState *tmp;

	tmp = (xmlValidState *) xmlRealloc(ctxt->vstateTab,
	             2 * ctxt->vstateMax * sizeof(ctxt->vstateTab[0]));
        if (tmp == NULL) {
	    xmlVErrMemory(ctxt, "realloc failed");
	    return(-1);
	}
	ctxt->vstateMax *= 2;
	ctxt->vstateTab = tmp;
    }
    ctxt->vstate = &ctxt->vstateTab[ctxt->vstateNr];
    ctxt->vstateTab[ctxt->vstateNr].elemDecl = elemDecl;
    ctxt->vstateTab[ctxt->vstateNr].node = node;
    if ((elemDecl != NULL) && (elemDecl->etype == XML_ELEMENT_TYPE_ELEMENT)) {
	if (elemDecl->contModel == NULL)
	    xmlValidBuildContentModel(ctxt, elemDecl);
	if (elemDecl->contModel != NULL) {
	    ctxt->vstateTab[ctxt->vstateNr].exec =
		xmlRegNewExecCtxt(elemDecl->contModel, NULL, NULL);
	} else {
	    ctxt->vstateTab[ctxt->vstateNr].exec = NULL;
	    xmlErrValidNode(ctxt, (xmlNodePtr) elemDecl,
	                    XML_ERR_INTERNAL_ERROR,
			    "Failed to build content model regexp for %s\n",
			    node->name, NULL, NULL);
	}
    }
    return(ctxt->vstateNr++);
}