void check_buffer(xmlParserInputPtr in) {
    if (in->base != xmlBufContent(in->buf->buffer)) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInput: base mismatch problem\n");
    }
    if (in->cur < in->base) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInput: cur < base problem\n");
    }
    if (in->cur > in->base + xmlBufUse(in->buf->buffer)) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInput: cur > base + use problem\n");
    }
    xmlGenericError(xmlGenericErrorContext,"buffer %x : content %x, cur %d, use %d\n",
            (int) in, (int) xmlBufContent(in->buf->buffer), in->cur - in->base,
	    xmlBufUse(in->buf->buffer));
}