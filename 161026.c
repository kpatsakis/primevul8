xmlEscapeEntities(unsigned char* out, int *outlen,
                 const xmlChar* in, int *inlen) {
    unsigned char* outstart = out;
    const unsigned char* base = in;
    unsigned char* outend = out + *outlen;
    const unsigned char* inend;
    int val;

    inend = in + (*inlen);

    while ((in < inend) && (out < outend)) {
	if (*in == '<') {
	    if (outend - out < 4) break;
	    *out++ = '&';
	    *out++ = 'l';
	    *out++ = 't';
	    *out++ = ';';
	    in++;
	    continue;
	} else if (*in == '>') {
	    if (outend - out < 4) break;
	    *out++ = '&';
	    *out++ = 'g';
	    *out++ = 't';
	    *out++ = ';';
	    in++;
	    continue;
	} else if (*in == '&') {
	    if (outend - out < 5) break;
	    *out++ = '&';
	    *out++ = 'a';
	    *out++ = 'm';
	    *out++ = 'p';
	    *out++ = ';';
	    in++;
	    continue;
	} else if (((*in >= 0x20) && (*in < 0x80)) ||
	           (*in == '\n') || (*in == '\t')) {
	    /*
	     * default case, just copy !
	     */
	    *out++ = *in++;
	    continue;
	} else if (*in >= 0x80) {
	    /*
	     * We assume we have UTF-8 input.
	     */
	    if (outend - out < 11) break;

	    if (*in < 0xC0) {
		xmlSaveErr(XML_SAVE_NOT_UTF8, NULL, NULL);
		in++;
		goto error;
	    } else if (*in < 0xE0) {
		if (inend - in < 2) break;
		val = (in[0]) & 0x1F;
		val <<= 6;
		val |= (in[1]) & 0x3F;
		in += 2;
	    } else if (*in < 0xF0) {
		if (inend - in < 3) break;
		val = (in[0]) & 0x0F;
		val <<= 6;
		val |= (in[1]) & 0x3F;
		val <<= 6;
		val |= (in[2]) & 0x3F;
		in += 3;
	    } else if (*in < 0xF8) {
		if (inend - in < 4) break;
		val = (in[0]) & 0x07;
		val <<= 6;
		val |= (in[1]) & 0x3F;
		val <<= 6;
		val |= (in[2]) & 0x3F;
		val <<= 6;
		val |= (in[3]) & 0x3F;
		in += 4;
	    } else {
		xmlSaveErr(XML_SAVE_CHAR_INVALID, NULL, NULL);
		in++;
		goto error;
	    }
	    if (!IS_CHAR(val)) {
		xmlSaveErr(XML_SAVE_CHAR_INVALID, NULL, NULL);
		in++;
		goto error;
	    }

	    /*
	     * We could do multiple things here. Just save as a char ref
	     */
	    out = xmlSerializeHexCharRef(out, val);
	} else if (IS_BYTE_CHAR(*in)) {
	    if (outend - out < 6) break;
	    out = xmlSerializeHexCharRef(out, *in++);
	} else {
	    xmlGenericError(xmlGenericErrorContext,
		"xmlEscapeEntities : char out of range\n");
	    in++;
	    goto error;
	}
    }
    *outlen = out - outstart;
    *inlen = in - base;
    return(0);
error:
    *outlen = out - outstart;
    *inlen = in - base;
    return(-1);
}