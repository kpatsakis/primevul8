htmlParseLookupChars(htmlParserCtxtPtr ctxt, const xmlChar * stop,
                     int stopLen)
{
    int base, len;
    htmlParserInputPtr in;
    const xmlChar *buf;
    int incomment = 0;
    int i;

    in = ctxt->input;
    if (in == NULL)
        return (-1);

    base = in->cur - in->base;
    if (base < 0)
        return (-1);

    if (ctxt->checkIndex > base)
        base = ctxt->checkIndex;

    if (in->buf == NULL) {
        buf = in->base;
        len = in->length;
    } else {
        buf = xmlBufContent(in->buf->buffer);
        len = xmlBufUse(in->buf->buffer);
    }

    for (; base < len; base++) {
        if (!incomment && (base + 4 < len)) {
            if ((buf[base] == '<') && (buf[base + 1] == '!') &&
                (buf[base + 2] == '-') && (buf[base + 3] == '-')) {
                incomment = 1;
                /* do not increment past <! - some people use <!--> */
                base += 2;
            }
        }
        if (incomment) {
            if (base + 3 > len)
                return (-1);
            if ((buf[base] == '-') && (buf[base + 1] == '-') &&
                (buf[base + 2] == '>')) {
                incomment = 0;
                base += 2;
            }
            continue;
        }
        for (i = 0; i < stopLen; ++i) {
            if (buf[base] == stop[i]) {
                ctxt->checkIndex = 0;
                return (base - (in->cur - in->base));
            }
        }
    }
    ctxt->checkIndex = base;
    return (-1);
}