vstateVPush(xmlValidCtxtPtr ctxt, xmlElementContentPtr cont,
	    xmlNodePtr node, unsigned char depth, long occurs,
	    unsigned char state) {
    int i = ctxt->vstateNr - 1;

    if (ctxt->vstateNr > MAX_RECURSE) {
	return(-1);
    }
    if (ctxt->vstateTab == NULL) {
	ctxt->vstateMax = 8;
	ctxt->vstateTab = (xmlValidState *) xmlMalloc(
		     ctxt->vstateMax * sizeof(ctxt->vstateTab[0]));
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
	    xmlVErrMemory(ctxt, "malloc failed");
	    return(-1);
	}
	ctxt->vstateMax *= 2;
	ctxt->vstateTab = tmp;
	ctxt->vstate = &ctxt->vstateTab[0];
    }
    /*
     * Don't push on the stack a state already here
     */
    if ((i >= 0) && (ctxt->vstateTab[i].cont == cont) &&
	(ctxt->vstateTab[i].node == node) &&
	(ctxt->vstateTab[i].depth == depth) &&
	(ctxt->vstateTab[i].occurs == occurs) &&
	(ctxt->vstateTab[i].state == state))
	return(ctxt->vstateNr);
    ctxt->vstateTab[ctxt->vstateNr].cont = cont;
    ctxt->vstateTab[ctxt->vstateNr].node = node;
    ctxt->vstateTab[ctxt->vstateNr].depth = depth;
    ctxt->vstateTab[ctxt->vstateNr].occurs = occurs;
    ctxt->vstateTab[ctxt->vstateNr].state = state;
    return(ctxt->vstateNr++);
}