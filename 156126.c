xmlParseElementEnd(xmlParserCtxtPtr ctxt) {
    xmlParserNodeInfo node_info;
    xmlNodePtr ret = ctxt->node;

    if (ctxt->nameNr <= 0)
        return;

    /*
     * parse the end of tag: '</' should be here.
     */
    if (ctxt->sax2) {
        const xmlChar *prefix = ctxt->pushTab[ctxt->nameNr * 3 - 3];
        const xmlChar *URI = ctxt->pushTab[ctxt->nameNr * 3 - 2];
        int nsNr = (ptrdiff_t) ctxt->pushTab[ctxt->nameNr * 3 - 1];
	xmlParseEndTag2(ctxt, prefix, URI, 0, nsNr, 0);
	namePop(ctxt);
    }
#ifdef LIBXML_SAX1_ENABLED
    else
	xmlParseEndTag1(ctxt, 0);
#endif /* LIBXML_SAX1_ENABLED */

    /*
     * Capture end position and add node
     */
    if ( ret != NULL && ctxt->record_info ) {
       node_info.end_pos = ctxt->input->consumed +
                          (CUR_PTR - ctxt->input->base);
       node_info.end_line = ctxt->input->line;
       node_info.node = ret;
       xmlParserAddNodeInfo(ctxt, &node_info);
    }
}