xmlValidDebugState(xmlValidStatePtr state) {
    xmlGenericError(xmlGenericErrorContext, "(");
    if (state->cont == NULL)
	xmlGenericError(xmlGenericErrorContext, "null,");
    else
	switch (state->cont->type) {
            case XML_ELEMENT_CONTENT_PCDATA:
		xmlGenericError(xmlGenericErrorContext, "pcdata,");
		break;
            case XML_ELEMENT_CONTENT_ELEMENT:
		xmlGenericError(xmlGenericErrorContext, "%s,",
			        state->cont->name);
		break;
            case XML_ELEMENT_CONTENT_SEQ:
		xmlGenericError(xmlGenericErrorContext, "seq,");
		break;
            case XML_ELEMENT_CONTENT_OR:
		xmlGenericError(xmlGenericErrorContext, "or,");
		break;
	}
    xmlValidPrintNode(state->node);
    xmlGenericError(xmlGenericErrorContext, ",%d,%X,%d)",
	    state->depth, state->occurs, state->state);
}