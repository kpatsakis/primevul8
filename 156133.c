xmlParseContent(xmlParserCtxtPtr ctxt) {
    int nameNr = ctxt->nameNr;

    GROW;
    while ((RAW != 0) &&
	   (ctxt->instate != XML_PARSER_EOF)) {
	const xmlChar *test = CUR_PTR;
	unsigned int cons = ctxt->input->consumed;
	const xmlChar *cur = ctxt->input->cur;

	/*
	 * First case : a Processing Instruction.
	 */
	if ((*cur == '<') && (cur[1] == '?')) {
	    xmlParsePI(ctxt);
	}

	/*
	 * Second case : a CDSection
	 */
	/* 2.6.0 test was *cur not RAW */
	else if (CMP9(CUR_PTR, '<', '!', '[', 'C', 'D', 'A', 'T', 'A', '[')) {
	    xmlParseCDSect(ctxt);
	}

	/*
	 * Third case :  a comment
	 */
	else if ((*cur == '<') && (NXT(1) == '!') &&
		 (NXT(2) == '-') && (NXT(3) == '-')) {
	    xmlParseComment(ctxt);
	    ctxt->instate = XML_PARSER_CONTENT;
	}

	/*
	 * Fourth case :  a sub-element.
	 */
	else if (*cur == '<') {
            if (NXT(1) == '/') {
                if (ctxt->nameNr <= nameNr)
                    break;
	        xmlParseElementEnd(ctxt);
            } else {
	        xmlParseElementStart(ctxt);
            }
	}

	/*
	 * Fifth case : a reference. If if has not been resolved,
	 *    parsing returns it's Name, create the node
	 */

	else if (*cur == '&') {
	    xmlParseReference(ctxt);
	}

	/*
	 * Last case, text. Note that References are handled directly.
	 */
	else {
	    xmlParseCharData(ctxt, 0);
	}

	GROW;
	SHRINK;

	if ((cons == ctxt->input->consumed) && (test == CUR_PTR)) {
	    xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR,
	                "detected an error in element content\n");
	    xmlHaltParser(ctxt);
            break;
	}
    }
}