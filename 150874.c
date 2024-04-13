vstateVPop(xmlValidCtxtPtr ctxt) {
    if (ctxt->vstateNr <= 1) return(-1);
    ctxt->vstateNr--;
    ctxt->vstate = &ctxt->vstateTab[0];
    ctxt->vstate->cont =  ctxt->vstateTab[ctxt->vstateNr].cont;
    ctxt->vstate->node = ctxt->vstateTab[ctxt->vstateNr].node;
    ctxt->vstate->depth = ctxt->vstateTab[ctxt->vstateNr].depth;
    ctxt->vstate->occurs = ctxt->vstateTab[ctxt->vstateNr].occurs;
    ctxt->vstate->state = ctxt->vstateTab[ctxt->vstateNr].state;
    return(ctxt->vstateNr);
}