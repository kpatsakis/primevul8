xmlSwitchInputEncodingInt(xmlParserCtxtPtr ctxt, xmlParserInputPtr input,
                          xmlCharEncodingHandlerPtr handler, int len)
{
    int nbchars;

    if (handler == NULL)
        return (-1);
    if (input == NULL)
        return (-1);
    if (input->buf != NULL) {
        if (input->buf->encoder != NULL) {
            /*
             * Check in case the auto encoding detetection triggered
             * in already.
             */
            if (input->buf->encoder == handler)
                return (0);

            /*
             * "UTF-16" can be used for both LE and BE
             if ((!xmlStrncmp(BAD_CAST input->buf->encoder->name,
             BAD_CAST "UTF-16", 6)) &&
             (!xmlStrncmp(BAD_CAST handler->name,
             BAD_CAST "UTF-16", 6))) {
             return(0);
             }
             */

            /*
             * Note: this is a bit dangerous, but that's what it
             * takes to use nearly compatible signature for different
             * encodings.
             */
            xmlCharEncCloseFunc(input->buf->encoder);
            input->buf->encoder = handler;
            return (0);
        }
        input->buf->encoder = handler;

        /*
         * Is there already some content down the pipe to convert ?
         */
        if (xmlBufIsEmpty(input->buf->buffer) == 0) {
            int processed;
	    unsigned int use;

            /*
             * Specific handling of the Byte Order Mark for
             * UTF-16
             */
            if ((handler->name != NULL) &&
                (!strcmp(handler->name, "UTF-16LE") ||
                 !strcmp(handler->name, "UTF-16")) &&
                (input->cur[0] == 0xFF) && (input->cur[1] == 0xFE)) {
                input->cur += 2;
            }
            if ((handler->name != NULL) &&
                (!strcmp(handler->name, "UTF-16BE")) &&
                (input->cur[0] == 0xFE) && (input->cur[1] == 0xFF)) {
                input->cur += 2;
            }
            /*
             * Errata on XML-1.0 June 20 2001
             * Specific handling of the Byte Order Mark for
             * UTF-8
             */
            if ((handler->name != NULL) &&
                (!strcmp(handler->name, "UTF-8")) &&
                (input->cur[0] == 0xEF) &&
                (input->cur[1] == 0xBB) && (input->cur[2] == 0xBF)) {
                input->cur += 3;
            }

            /*
             * Shrink the current input buffer.
             * Move it as the raw buffer and create a new input buffer
             */
            processed = input->cur - input->base;
            xmlBufShrink(input->buf->buffer, processed);
            input->buf->raw = input->buf->buffer;
            input->buf->buffer = xmlBufCreate();
	    input->buf->rawconsumed = processed;
	    use = xmlBufUse(input->buf->raw);

            if (ctxt->html) {
                /*
                 * convert as much as possible of the buffer
                 */
                nbchars = xmlCharEncInput(input->buf, 1);
            } else {
                /*
                 * convert just enough to get
                 * '<?xml version="1.0" encoding="xxx"?>'
                 * parsed with the autodetected encoding
                 * into the parser reading buffer.
                 */
                nbchars = xmlCharEncFirstLineInput(input->buf, len);
            }
            if (nbchars < 0) {
                xmlErrInternal(ctxt,
                               "switching encoding: encoder error\n",
                               NULL);
                return (-1);
            }
	    input->buf->rawconsumed += use - xmlBufUse(input->buf->raw);
            xmlBufResetInput(input->buf->buffer, input);
        }
        return (0);
    } else if (input->length == 0) {
	/*
	 * When parsing a static memory array one must know the
	 * size to be able to convert the buffer.
	 */
	xmlErrInternal(ctxt, "switching encoding : no input\n", NULL);
	return (-1);
    }
    return (0);
}