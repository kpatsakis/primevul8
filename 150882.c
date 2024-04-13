xmlIsDocNameStartChar(xmlDocPtr doc, int c) {
    if ((doc == NULL) || (doc->properties & XML_DOC_OLD10) == 0) {
        /*
	 * Use the new checks of production [4] [4a] amd [5] of the
	 * Update 5 of XML-1.0
	 */
	if (((c >= 'a') && (c <= 'z')) ||
	    ((c >= 'A') && (c <= 'Z')) ||
	    (c == '_') || (c == ':') ||
	    ((c >= 0xC0) && (c <= 0xD6)) ||
	    ((c >= 0xD8) && (c <= 0xF6)) ||
	    ((c >= 0xF8) && (c <= 0x2FF)) ||
	    ((c >= 0x370) && (c <= 0x37D)) ||
	    ((c >= 0x37F) && (c <= 0x1FFF)) ||
	    ((c >= 0x200C) && (c <= 0x200D)) ||
	    ((c >= 0x2070) && (c <= 0x218F)) ||
	    ((c >= 0x2C00) && (c <= 0x2FEF)) ||
	    ((c >= 0x3001) && (c <= 0xD7FF)) ||
	    ((c >= 0xF900) && (c <= 0xFDCF)) ||
	    ((c >= 0xFDF0) && (c <= 0xFFFD)) ||
	    ((c >= 0x10000) && (c <= 0xEFFFF)))
	    return(1);
    } else {
        if (IS_LETTER(c) || (c == '_') || (c == ':'))
	    return(1);
    }
    return(0);
}