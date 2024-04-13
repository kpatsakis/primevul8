htmlCheckParagraph(htmlParserCtxtPtr ctxt) {
    const xmlChar *tag;
    int i;

    if (ctxt == NULL)
	return(-1);
    tag = ctxt->name;
    if (tag == NULL) {
	htmlAutoClose(ctxt, BAD_CAST"p");
	htmlCheckImplied(ctxt, BAD_CAST"p");
	htmlnamePush(ctxt, BAD_CAST"p");
	if ((ctxt->sax != NULL) && (ctxt->sax->startElement != NULL))
	    ctxt->sax->startElement(ctxt->userData, BAD_CAST"p", NULL);
	return(1);
    }
    if (!htmlOmittedDefaultValue)
	return(0);
    for (i = 0; htmlNoContentElements[i] != NULL; i++) {
	if (xmlStrEqual(tag, BAD_CAST htmlNoContentElements[i])) {
	    htmlAutoClose(ctxt, BAD_CAST"p");
	    htmlCheckImplied(ctxt, BAD_CAST"p");
	    htmlnamePush(ctxt, BAD_CAST"p");
	    if ((ctxt->sax != NULL) && (ctxt->sax->startElement != NULL))
		ctxt->sax->startElement(ctxt->userData, BAD_CAST"p", NULL);
	    return(1);
	}
    }
    return(0);
}