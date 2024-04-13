htmlCheckEncoding(htmlParserCtxtPtr ctxt, const xmlChar *attvalue) {
    const xmlChar *encoding;

    if (!attvalue)
	return;

    encoding = xmlStrcasestr(attvalue, BAD_CAST"charset");
    if (encoding != NULL) {
	encoding += 7;
    }
    /*
     * skip blank
     */
    if (encoding && IS_BLANK_CH(*encoding))
	encoding = xmlStrcasestr(attvalue, BAD_CAST"=");
    if (encoding && *encoding == '=') {
	encoding ++;
	htmlCheckEncodingDirect(ctxt, encoding);
    }
}