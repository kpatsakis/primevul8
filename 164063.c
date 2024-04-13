htmlParseEntityRef(htmlParserCtxtPtr ctxt, const xmlChar **str) {
    const xmlChar *name;
    const htmlEntityDesc * ent = NULL;

    if (str != NULL) *str = NULL;
    if ((ctxt == NULL) || (ctxt->input == NULL)) return(NULL);

    if (CUR == '&') {
        NEXT;
        name = htmlParseName(ctxt);
	if (name == NULL) {
	    htmlParseErr(ctxt, XML_ERR_NAME_REQUIRED,
	                 "htmlParseEntityRef: no name\n", NULL, NULL);
	} else {
	    GROW;
	    if (CUR == ';') {
	        if (str != NULL)
		    *str = name;

		/*
		 * Lookup the entity in the table.
		 */
		ent = htmlEntityLookup(name);
		if (ent != NULL) /* OK that's ugly !!! */
		    NEXT;
	    } else {
		htmlParseErr(ctxt, XML_ERR_ENTITYREF_SEMICOL_MISSING,
		             "htmlParseEntityRef: expecting ';'\n",
			     NULL, NULL);
	        if (str != NULL)
		    *str = name;
	    }
	}
    }
    return(ent);
}