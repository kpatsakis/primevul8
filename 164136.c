htmlParseExternalID(htmlParserCtxtPtr ctxt, xmlChar **publicID) {
    xmlChar *URI = NULL;

    if ((UPPER == 'S') && (UPP(1) == 'Y') &&
         (UPP(2) == 'S') && (UPP(3) == 'T') &&
	 (UPP(4) == 'E') && (UPP(5) == 'M')) {
        SKIP(6);
	if (!IS_BLANK_CH(CUR)) {
	    htmlParseErr(ctxt, XML_ERR_SPACE_REQUIRED,
	                 "Space required after 'SYSTEM'\n", NULL, NULL);
	}
        SKIP_BLANKS;
	URI = htmlParseSystemLiteral(ctxt);
	if (URI == NULL) {
	    htmlParseErr(ctxt, XML_ERR_URI_REQUIRED,
	                 "htmlParseExternalID: SYSTEM, no URI\n", NULL, NULL);
        }
    } else if ((UPPER == 'P') && (UPP(1) == 'U') &&
	       (UPP(2) == 'B') && (UPP(3) == 'L') &&
	       (UPP(4) == 'I') && (UPP(5) == 'C')) {
        SKIP(6);
	if (!IS_BLANK_CH(CUR)) {
	    htmlParseErr(ctxt, XML_ERR_SPACE_REQUIRED,
	                 "Space required after 'PUBLIC'\n", NULL, NULL);
	}
        SKIP_BLANKS;
	*publicID = htmlParsePubidLiteral(ctxt);
	if (*publicID == NULL) {
	    htmlParseErr(ctxt, XML_ERR_PUBID_REQUIRED,
	                 "htmlParseExternalID: PUBLIC, no Public Identifier\n",
			 NULL, NULL);
	}
        SKIP_BLANKS;
        if ((CUR == '"') || (CUR == '\'')) {
	    URI = htmlParseSystemLiteral(ctxt);
	}
    }
    return(URI);
}