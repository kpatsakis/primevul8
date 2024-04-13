htmlParseHTMLName_nonInvasive(htmlParserCtxtPtr ctxt) {
    int i = 0;
    xmlChar loc[HTML_PARSER_BUFFER_SIZE];

    if (!IS_ASCII_LETTER(NXT(1)) && (NXT(1) != '_') &&
        (NXT(1) != ':')) return(NULL);

    while ((i < HTML_PARSER_BUFFER_SIZE) &&
           ((IS_ASCII_LETTER(NXT(1+i))) || (IS_ASCII_DIGIT(NXT(1+i))) ||
	   (NXT(1+i) == ':') || (NXT(1+i) == '-') || (NXT(1+i) == '_'))) {
	if ((NXT(1+i) >= 'A') && (NXT(1+i) <= 'Z')) loc[i] = NXT(1+i) + 0x20;
        else loc[i] = NXT(1+i);
	i++;
    }

    return(xmlDictLookup(ctxt->dict, loc, i));
}