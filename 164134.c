htmlParseCharRef(htmlParserCtxtPtr ctxt) {
    int val = 0;

    if ((ctxt == NULL) || (ctxt->input == NULL)) {
	htmlParseErr(ctxt, XML_ERR_INTERNAL_ERROR,
		     "htmlParseCharRef: context error\n",
		     NULL, NULL);
        return(0);
    }
    if ((CUR == '&') && (NXT(1) == '#') &&
        ((NXT(2) == 'x') || NXT(2) == 'X')) {
	SKIP(3);
	while (CUR != ';') {
	    if ((CUR >= '0') && (CUR <= '9'))
	        val = val * 16 + (CUR - '0');
	    else if ((CUR >= 'a') && (CUR <= 'f'))
	        val = val * 16 + (CUR - 'a') + 10;
	    else if ((CUR >= 'A') && (CUR <= 'F'))
	        val = val * 16 + (CUR - 'A') + 10;
	    else {
	        htmlParseErr(ctxt, XML_ERR_INVALID_HEX_CHARREF,
		             "htmlParseCharRef: missing semicolon\n",
			     NULL, NULL);
		break;
	    }
	    NEXT;
	}
	if (CUR == ';')
	    NEXT;
    } else if  ((CUR == '&') && (NXT(1) == '#')) {
	SKIP(2);
	while (CUR != ';') {
	    if ((CUR >= '0') && (CUR <= '9'))
	        val = val * 10 + (CUR - '0');
	    else {
	        htmlParseErr(ctxt, XML_ERR_INVALID_DEC_CHARREF,
		             "htmlParseCharRef: missing semicolon\n",
			     NULL, NULL);
		break;
	    }
	    NEXT;
	}
	if (CUR == ';')
	    NEXT;
    } else {
	htmlParseErr(ctxt, XML_ERR_INVALID_CHARREF,
	             "htmlParseCharRef: invalid value\n", NULL, NULL);
    }
    /*
     * Check the value IS_CHAR ...
     */
    if (IS_CHAR(val)) {
        return(val);
    } else {
	htmlParseErrInt(ctxt, XML_ERR_INVALID_CHAR,
			"htmlParseCharRef: invalid xmlChar value %d\n",
			val);
    }
    return(0);
}