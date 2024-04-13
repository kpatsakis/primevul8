static int areBlanks(htmlParserCtxtPtr ctxt, const xmlChar *str, int len) {
    unsigned int i;
    int j;
    xmlNodePtr lastChild;
    xmlDtdPtr dtd;

    for (j = 0;j < len;j++)
        if (!(IS_BLANK_CH(str[j]))) return(0);

    if (CUR == 0) return(1);
    if (CUR != '<') return(0);
    if (ctxt->name == NULL)
	return(1);
    if (xmlStrEqual(ctxt->name, BAD_CAST"html"))
	return(1);
    if (xmlStrEqual(ctxt->name, BAD_CAST"head"))
	return(1);

    /* Only strip CDATA children of the body tag for strict HTML DTDs */
    if (xmlStrEqual(ctxt->name, BAD_CAST "body") && ctxt->myDoc != NULL) {
        dtd = xmlGetIntSubset(ctxt->myDoc);
        if (dtd != NULL && dtd->ExternalID != NULL) {
            if (!xmlStrcasecmp(dtd->ExternalID, BAD_CAST "-//W3C//DTD HTML 4.01//EN") ||
                    !xmlStrcasecmp(dtd->ExternalID, BAD_CAST "-//W3C//DTD HTML 4//EN"))
                return(1);
        }
    }

    if (ctxt->node == NULL) return(0);
    lastChild = xmlGetLastChild(ctxt->node);
    while ((lastChild) && (lastChild->type == XML_COMMENT_NODE))
	lastChild = lastChild->prev;
    if (lastChild == NULL) {
        if ((ctxt->node->type != XML_ELEMENT_NODE) &&
            (ctxt->node->content != NULL)) return(0);
	/* keep ws in constructs like ...<b> </b>...
	   for all tags "b" allowing PCDATA */
	for ( i = 0; i < sizeof(allowPCData)/sizeof(allowPCData[0]); i++ ) {
	    if ( xmlStrEqual(ctxt->name, BAD_CAST allowPCData[i]) ) {
		return(0);
	    }
	}
    } else if (xmlNodeIsText(lastChild)) {
        return(0);
    } else {
	/* keep ws in constructs like <p><b>xy</b> <i>z</i><p>
	   for all tags "p" allowing PCDATA */
	for ( i = 0; i < sizeof(allowPCData)/sizeof(allowPCData[0]); i++ ) {
	    if ( xmlStrEqual(lastChild->name, BAD_CAST allowPCData[i]) ) {
		return(0);
	    }
	}
    }
    return(1);
}