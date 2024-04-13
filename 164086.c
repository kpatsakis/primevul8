htmlParseElement(htmlParserCtxtPtr ctxt) {
    const xmlChar *name;
    xmlChar *currentNode = NULL;
    const htmlElemDesc * info;
    htmlParserNodeInfo node_info;
    int failed;
    int depth;
    const xmlChar *oldptr;

    if ((ctxt == NULL) || (ctxt->input == NULL)) {
	htmlParseErr(ctxt, XML_ERR_INTERNAL_ERROR,
		     "htmlParseElement: context error\n", NULL, NULL);
	return;
    }

    if (ctxt->instate == XML_PARSER_EOF)
        return;

    /* Capture start position */
    if (ctxt->record_info) {
        node_info.begin_pos = ctxt->input->consumed +
                          (CUR_PTR - ctxt->input->base);
	node_info.begin_line = ctxt->input->line;
    }

    failed = htmlParseStartTag(ctxt);
    name = ctxt->name;
    if ((failed == -1) || (name == NULL)) {
	if (CUR == '>')
	    NEXT;
        return;
    }

    /*
     * Lookup the info for that element.
     */
    info = htmlTagLookup(name);
    if (info == NULL) {
	htmlParseErr(ctxt, XML_HTML_UNKNOWN_TAG,
	             "Tag %s invalid\n", name, NULL);
    }

    /*
     * Check for an Empty Element labeled the XML/SGML way
     */
    if ((CUR == '/') && (NXT(1) == '>')) {
        SKIP(2);
	if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
	    ctxt->sax->endElement(ctxt->userData, name);
	htmlnamePop(ctxt);
	return;
    }

    if (CUR == '>') {
        NEXT;
    } else {
	htmlParseErr(ctxt, XML_ERR_GT_REQUIRED,
	             "Couldn't find end of Start Tag %s\n", name, NULL);

	/*
	 * end of parsing of this node.
	 */
	if (xmlStrEqual(name, ctxt->name)) {
	    nodePop(ctxt);
	    htmlnamePop(ctxt);
	}

	/*
	 * Capture end position and add node
	 */
	if (ctxt->record_info) {
	   node_info.end_pos = ctxt->input->consumed +
			      (CUR_PTR - ctxt->input->base);
	   node_info.end_line = ctxt->input->line;
	   node_info.node = ctxt->node;
	   xmlParserAddNodeInfo(ctxt, &node_info);
	}
	return;
    }

    /*
     * Check for an Empty Element from DTD definition
     */
    if ((info != NULL) && (info->empty)) {
	if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
	    ctxt->sax->endElement(ctxt->userData, name);
	htmlnamePop(ctxt);
	return;
    }

    /*
     * Parse the content of the element:
     */
    currentNode = xmlStrdup(ctxt->name);
    depth = ctxt->nameNr;
    while (IS_CHAR_CH(CUR)) {
	oldptr = ctxt->input->cur;
	htmlParseContent(ctxt);
	if (oldptr==ctxt->input->cur) break;
	if (ctxt->nameNr < depth) break;
    }

    /*
     * Capture end position and add node
     */
    if ( currentNode != NULL && ctxt->record_info ) {
       node_info.end_pos = ctxt->input->consumed +
                          (CUR_PTR - ctxt->input->base);
       node_info.end_line = ctxt->input->line;
       node_info.node = ctxt->node;
       xmlParserAddNodeInfo(ctxt, &node_info);
    }
    if (!IS_CHAR_CH(CUR)) {
	htmlAutoCloseOnEnd(ctxt);
    }

    if (currentNode != NULL)
	xmlFree(currentNode);
}