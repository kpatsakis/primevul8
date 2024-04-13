xmlParserInputShrink(xmlParserInputPtr in) {
    size_t used;
    size_t ret;
    size_t indx;
    const xmlChar *content;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Shrink\n");
#endif
    if (in == NULL) return;
    if (in->buf == NULL) return;
    if (in->base == NULL) return;
    if (in->cur == NULL) return;
    if (in->buf->buffer == NULL) return;

    CHECK_BUFFER(in);

    used = in->cur - xmlBufContent(in->buf->buffer);
    /*
     * Do not shrink on large buffers whose only a tiny fraction
     * was consumed
     */
    if (used > INPUT_CHUNK) {
	ret = xmlBufShrink(in->buf->buffer, used - LINE_LEN);
	if (ret > 0) {
	    in->cur -= ret;
	    in->consumed += ret;
	}
	in->end = xmlBufEnd(in->buf->buffer);
    }

    CHECK_BUFFER(in);

    if (xmlBufUse(in->buf->buffer) > INPUT_CHUNK) {
        return;
    }
    xmlParserInputBufferRead(in->buf, 2 * INPUT_CHUNK);
    content = xmlBufContent(in->buf->buffer);
    if (in->base != content) {
        /*
	 * the buffer has been reallocated
	 */
	indx = in->cur - in->base;
	in->base = content;
	in->cur = &content[indx];
    }
    in->end = xmlBufEnd(in->buf->buffer);

    CHECK_BUFFER(in);
}