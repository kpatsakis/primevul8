htmlParseAttribute(htmlParserCtxtPtr ctxt, xmlChar **value) {
    const xmlChar *name;
    xmlChar *val = NULL;

    *value = NULL;
    name = htmlParseHTMLName(ctxt);
    if (name == NULL) {
	htmlParseErr(ctxt, XML_ERR_NAME_REQUIRED,
	             "error parsing attribute name\n", NULL, NULL);
        return(NULL);
    }

    /*
     * read the value
     */
    SKIP_BLANKS;
    if (CUR == '=') {
        NEXT;
	SKIP_BLANKS;
	val = htmlParseAttValue(ctxt);
    }

    *value = val;
    return(name);
}