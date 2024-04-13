htmlParseCharDataInternal(htmlParserCtxtPtr ctxt, int readahead) {
    xmlChar buf[HTML_PARSER_BIG_BUFFER_SIZE + 6];
    int nbchar = 0;
    int cur, l;
    int chunk = 0;

    if (readahead)
        buf[nbchar++] = readahead;

    SHRINK;
    cur = CUR_CHAR(l);
    while (((cur != '<') || (ctxt->token == '<')) &&
           ((cur != '&') || (ctxt->token == '&')) &&
	   (cur != 0)) {
	if (!(IS_CHAR(cur))) {
	    htmlParseErrInt(ctxt, XML_ERR_INVALID_CHAR,
	                "Invalid char in CDATA 0x%X\n", cur);
	} else {
	    COPY_BUF(l,buf,nbchar,cur);
	}
	if (nbchar >= HTML_PARSER_BIG_BUFFER_SIZE) {
	    /*
	     * Ok the segment is to be consumed as chars.
	     */
	    if ((ctxt->sax != NULL) && (!ctxt->disableSAX)) {
		if (areBlanks(ctxt, buf, nbchar)) {
		    if (ctxt->keepBlanks) {
			if (ctxt->sax->characters != NULL)
			    ctxt->sax->characters(ctxt->userData, buf, nbchar);
		    } else {
			if (ctxt->sax->ignorableWhitespace != NULL)
			    ctxt->sax->ignorableWhitespace(ctxt->userData,
			                                   buf, nbchar);
		    }
		} else {
		    htmlCheckParagraph(ctxt);
		    if (ctxt->sax->characters != NULL)
			ctxt->sax->characters(ctxt->userData, buf, nbchar);
		}
	    }
	    nbchar = 0;
	}
	NEXTL(l);
        chunk++;
        if (chunk > HTML_PARSER_BUFFER_SIZE) {
            chunk = 0;
            SHRINK;
            GROW;
        }
	cur = CUR_CHAR(l);
	if (cur == 0) {
	    SHRINK;
	    GROW;
	    cur = CUR_CHAR(l);
	}
    }
    if (nbchar != 0) {
        buf[nbchar] = 0;

	/*
	 * Ok the segment is to be consumed as chars.
	 */
	if ((ctxt->sax != NULL) && (!ctxt->disableSAX)) {
	    if (areBlanks(ctxt, buf, nbchar)) {
		if (ctxt->keepBlanks) {
		    if (ctxt->sax->characters != NULL)
			ctxt->sax->characters(ctxt->userData, buf, nbchar);
		} else {
		    if (ctxt->sax->ignorableWhitespace != NULL)
			ctxt->sax->ignorableWhitespace(ctxt->userData,
			                               buf, nbchar);
		}
	    } else {
		htmlCheckParagraph(ctxt);
		if (ctxt->sax->characters != NULL)
		    ctxt->sax->characters(ctxt->userData, buf, nbchar);
	    }
	}
    } else {
	/*
	 * Loop detection
	 */
	if (cur == 0)
	    ctxt->instate = XML_PARSER_EOF;
    }
}