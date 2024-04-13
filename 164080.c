htmlParseHTMLName(htmlParserCtxtPtr ctxt) {
    int i = 0;
    xmlChar loc[HTML_PARSER_BUFFER_SIZE];

    if (!IS_ASCII_LETTER(CUR) && (CUR != '_') &&
        (CUR != ':') && (CUR != '.')) return(NULL);

    while ((i < HTML_PARSER_BUFFER_SIZE) &&
           ((IS_ASCII_LETTER(CUR)) || (IS_ASCII_DIGIT(CUR)) ||
	   (CUR == ':') || (CUR == '-') || (CUR == '_') ||
           (CUR == '.'))) {
	if ((CUR >= 'A') && (CUR <= 'Z')) loc[i] = CUR + 0x20;
        else loc[i] = CUR;
	i++;

	NEXT;
    }

    return(xmlDictLookup(ctxt->dict, loc, i));
}