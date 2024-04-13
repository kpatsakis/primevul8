htmlCheckMeta(htmlParserCtxtPtr ctxt, const xmlChar **atts) {
    int i;
    const xmlChar *att, *value;
    int http = 0;
    const xmlChar *content = NULL;

    if ((ctxt == NULL) || (atts == NULL))
	return;

    i = 0;
    att = atts[i++];
    while (att != NULL) {
	value = atts[i++];
	if ((value != NULL) && (!xmlStrcasecmp(att, BAD_CAST"http-equiv"))
	 && (!xmlStrcasecmp(value, BAD_CAST"Content-Type")))
	    http = 1;
	else if ((value != NULL) && (!xmlStrcasecmp(att, BAD_CAST"charset")))
	    htmlCheckEncodingDirect(ctxt, value);
	else if ((value != NULL) && (!xmlStrcasecmp(att, BAD_CAST"content")))
	    content = value;
	att = atts[i++];
    }
    if ((http) && (content != NULL))
	htmlCheckEncoding(ctxt, content);

}