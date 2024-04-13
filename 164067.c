htmlParseScript(htmlParserCtxtPtr ctxt) {
    xmlChar buf[HTML_PARSER_BIG_BUFFER_SIZE + 5];
    int nbchar = 0;
    int cur,l;

    SHRINK;
    cur = CUR_CHAR(l);
    while (IS_CHAR_CH(cur)) {
	if ((cur == '<') && (NXT(1) == '/')) {
            /*
             * One should break here, the specification is clear:
             * Authors should therefore escape "</" within the content.
             * Escape mechanisms are specific to each scripting or
             * style sheet language.
             *
             * In recovery mode, only break if end tag match the
             * current tag, effectively ignoring all tags inside the
             * script/style block and treating the entire block as
             * CDATA.
             */
            if (ctxt->recovery) {
                if (xmlStrncasecmp(ctxt->name, ctxt->input->cur+2,
				   xmlStrlen(ctxt->name)) == 0)
                {
                    break; /* while */
                } else {
		    htmlParseErr(ctxt, XML_ERR_TAG_NAME_MISMATCH,
				 "Element %s embeds close tag\n",
		                 ctxt->name, NULL);
		}
            } else {
                if (((NXT(2) >= 'A') && (NXT(2) <= 'Z')) ||
                    ((NXT(2) >= 'a') && (NXT(2) <= 'z')))
                {
                    break; /* while */
                }
            }
	}
	COPY_BUF(l,buf,nbchar,cur);
	if (nbchar >= HTML_PARSER_BIG_BUFFER_SIZE) {
	    if (ctxt->sax->cdataBlock!= NULL) {
		/*
		 * Insert as CDATA, which is the same as HTML_PRESERVE_NODE
		 */
		ctxt->sax->cdataBlock(ctxt->userData, buf, nbchar);
	    } else if (ctxt->sax->characters != NULL) {
		ctxt->sax->characters(ctxt->userData, buf, nbchar);
	    }
	    nbchar = 0;
	}
	GROW;
	NEXTL(l);
	cur = CUR_CHAR(l);
    }

    if ((!(IS_CHAR_CH(cur))) && (!((cur == 0) && (ctxt->progressive)))) {
        htmlParseErrInt(ctxt, XML_ERR_INVALID_CHAR,
                    "Invalid char in CDATA 0x%X\n", cur);
        if (ctxt->input->cur < ctxt->input->end) {
            NEXT;
        }
    }

    if ((nbchar != 0) && (ctxt->sax != NULL) && (!ctxt->disableSAX)) {
	if (ctxt->sax->cdataBlock!= NULL) {
	    /*
	     * Insert as CDATA, which is the same as HTML_PRESERVE_NODE
	     */
	    ctxt->sax->cdataBlock(ctxt->userData, buf, nbchar);
	} else if (ctxt->sax->characters != NULL) {
	    ctxt->sax->characters(ctxt->userData, buf, nbchar);
	}
    }
}