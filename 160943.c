xmlParserInputGrow(xmlParserInputPtr in, int len) {
    size_t ret;
    size_t indx;
    const xmlChar *content;

    if ((in == NULL) || (len < 0)) return(-1);
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Grow\n");
#endif
    if (in->buf == NULL) return(-1);
    if (in->base == NULL) return(-1);
    if (in->cur == NULL) return(-1);
    if (in->buf->buffer == NULL) return(-1);

    CHECK_BUFFER(in);

    indx = in->cur - in->base;
    if (xmlBufUse(in->buf->buffer) > (unsigned int) indx + INPUT_CHUNK) {

	CHECK_BUFFER(in);

        return(0);
    }
    if (in->buf->readcallback != NULL) {
	ret = xmlParserInputBufferGrow(in->buf, len);
    } else
        return(0);

    /*
     * NOTE : in->base may be a "dangling" i.e. freed pointer in this
     *        block, but we use it really as an integer to do some
     *        pointer arithmetic. Insure will raise it as a bug but in
     *        that specific case, that's not !
     */

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

    return(ret);
}