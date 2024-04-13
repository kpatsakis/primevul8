xmlValidStateDebug(xmlValidCtxtPtr ctxt) {
    int i, j;

    xmlGenericError(xmlGenericErrorContext, "state: ");
    xmlValidDebugState(ctxt->vstate);
    xmlGenericError(xmlGenericErrorContext, " stack: %d ",
	    ctxt->vstateNr - 1);
    for (i = 0, j = ctxt->vstateNr - 1;(i < 3) && (j > 0);i++,j--)
	xmlValidDebugState(&ctxt->vstateTab[j]);
    xmlGenericError(xmlGenericErrorContext, "\n");
}