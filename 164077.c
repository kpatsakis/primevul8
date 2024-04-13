htmlParsePI(htmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = HTML_PARSER_BUFFER_SIZE;
    int cur, l;
    const xmlChar *target;
    xmlParserInputState state;
    int count = 0;

    if ((RAW == '<') && (NXT(1) == '?')) {
	state = ctxt->instate;
        ctxt->instate = XML_PARSER_PI;
	/*
	 * this is a Processing Instruction.
	 */
	SKIP(2);
	SHRINK;

	/*
	 * Parse the target name and check for special support like
	 * namespace.
	 */
        target = htmlParseName(ctxt);
	if (target != NULL) {
	    if (RAW == '>') {
		SKIP(1);

		/*
		 * SAX: PI detected.
		 */
		if ((ctxt->sax) && (!ctxt->disableSAX) &&
		    (ctxt->sax->processingInstruction != NULL))
		    ctxt->sax->processingInstruction(ctxt->userData,
		                                     target, NULL);
		ctxt->instate = state;
		return;
	    }
	    buf = (xmlChar *) xmlMallocAtomic(size * sizeof(xmlChar));
	    if (buf == NULL) {
		htmlErrMemory(ctxt, NULL);
		ctxt->instate = state;
		return;
	    }
	    cur = CUR;
	    if (!IS_BLANK(cur)) {
		htmlParseErr(ctxt, XML_ERR_SPACE_REQUIRED,
			  "ParsePI: PI %s space expected\n", target, NULL);
	    }
            SKIP_BLANKS;
	    cur = CUR_CHAR(l);
	    while (IS_CHAR(cur) && (cur != '>')) {
		if (len + 5 >= size) {
		    xmlChar *tmp;

		    size *= 2;
		    tmp = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
		    if (tmp == NULL) {
			htmlErrMemory(ctxt, NULL);
			xmlFree(buf);
			ctxt->instate = state;
			return;
		    }
		    buf = tmp;
		}
		count++;
		if (count > 50) {
		    GROW;
		    count = 0;
		}
		COPY_BUF(l,buf,len,cur);
		NEXTL(l);
		cur = CUR_CHAR(l);
		if (cur == 0) {
		    SHRINK;
		    GROW;
		    cur = CUR_CHAR(l);
		}
	    }
	    buf[len] = 0;
	    if (cur != '>') {
		htmlParseErr(ctxt, XML_ERR_PI_NOT_FINISHED,
		      "ParsePI: PI %s never end ...\n", target, NULL);
	    } else {
		SKIP(1);

		/*
		 * SAX: PI detected.
		 */
		if ((ctxt->sax) && (!ctxt->disableSAX) &&
		    (ctxt->sax->processingInstruction != NULL))
		    ctxt->sax->processingInstruction(ctxt->userData,
		                                     target, buf);
	    }
	    xmlFree(buf);
	} else {
	    htmlParseErr(ctxt, XML_ERR_PI_NOT_STARTED,
                         "PI is not started correctly", NULL, NULL);
	}
	ctxt->instate = state;
    }
}