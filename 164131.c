htmlParseContent(htmlParserCtxtPtr ctxt) {
    xmlChar *currentNode;
    int depth;
    const xmlChar *name;

    currentNode = xmlStrdup(ctxt->name);
    depth = ctxt->nameNr;
    while (1) {
	long cons = ctxt->nbChars;

        GROW;

        if (ctxt->instate == XML_PARSER_EOF)
            break;

	/*
	 * Our tag or one of it's parent or children is ending.
	 */
        if ((CUR == '<') && (NXT(1) == '/')) {
	    if (htmlParseEndTag(ctxt) &&
		((currentNode != NULL) || (ctxt->nameNr == 0))) {
		if (currentNode != NULL)
		    xmlFree(currentNode);
		return;
	    }
	    continue; /* while */
        }

	else if ((CUR == '<') &&
	         ((IS_ASCII_LETTER(NXT(1))) ||
		  (NXT(1) == '_') || (NXT(1) == ':'))) {
	    name = htmlParseHTMLName_nonInvasive(ctxt);
	    if (name == NULL) {
	        htmlParseErr(ctxt, XML_ERR_NAME_REQUIRED,
			 "htmlParseStartTag: invalid element name\n",
			 NULL, NULL);
	        /* Dump the bogus tag like browsers do */
        while ((IS_CHAR_CH(CUR)) && (CUR != '>'))
	            NEXT;

	        if (currentNode != NULL)
	            xmlFree(currentNode);
	        return;
	    }

	    if (ctxt->name != NULL) {
	        if (htmlCheckAutoClose(name, ctxt->name) == 1) {
	            htmlAutoClose(ctxt, name);
	            continue;
	        }
	    }
	}

	/*
	 * Has this node been popped out during parsing of
	 * the next element
	 */
        if ((ctxt->nameNr > 0) && (depth >= ctxt->nameNr) &&
	    (!xmlStrEqual(currentNode, ctxt->name)))
	     {
	    if (currentNode != NULL) xmlFree(currentNode);
	    return;
	}

	if ((CUR != 0) && ((xmlStrEqual(currentNode, BAD_CAST"script")) ||
	    (xmlStrEqual(currentNode, BAD_CAST"style")))) {
	    /*
	     * Handle SCRIPT/STYLE separately
	     */
	    htmlParseScript(ctxt);
	} else {
	    /*
	     * Sometimes DOCTYPE arrives in the middle of the document
	     */
	    if ((CUR == '<') && (NXT(1) == '!') &&
		(UPP(2) == 'D') && (UPP(3) == 'O') &&
		(UPP(4) == 'C') && (UPP(5) == 'T') &&
		(UPP(6) == 'Y') && (UPP(7) == 'P') &&
		(UPP(8) == 'E')) {
		htmlParseErr(ctxt, XML_HTML_STRUCURE_ERROR,
		             "Misplaced DOCTYPE declaration\n",
			     BAD_CAST "DOCTYPE" , NULL);
		htmlParseDocTypeDecl(ctxt);
	    }

	    /*
	     * First case :  a comment
	     */
	    if ((CUR == '<') && (NXT(1) == '!') &&
		(NXT(2) == '-') && (NXT(3) == '-')) {
		htmlParseComment(ctxt);
	    }

	    /*
	     * Second case : a Processing Instruction.
	     */
	    else if ((CUR == '<') && (NXT(1) == '?')) {
		htmlParsePI(ctxt);
	    }

	    /*
	     * Third case :  a sub-element.
	     */
	    else if (CUR == '<') {
		htmlParseElement(ctxt);
	    }

	    /*
	     * Fourth case : a reference. If if has not been resolved,
	     *    parsing returns it's Name, create the node
	     */
	    else if (CUR == '&') {
		htmlParseReference(ctxt);
	    }

	    /*
	     * Fifth case : end of the resource
	     */
	    else if (CUR == 0) {
		htmlAutoCloseOnEnd(ctxt);
		break;
	    }

	    /*
	     * Last case, text. Note that References are handled directly.
	     */
	    else {
		htmlParseCharData(ctxt);
	    }

	    if (cons == ctxt->nbChars) {
		if (ctxt->node != NULL) {
		    htmlParseErr(ctxt, XML_ERR_INTERNAL_ERROR,
		                 "detected an error in element content\n",
				 NULL, NULL);
		}
		break;
	    }
	}
        GROW;
    }
    if (currentNode != NULL) xmlFree(currentNode);
}