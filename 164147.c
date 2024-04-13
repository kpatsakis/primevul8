htmlParseHTMLAttribute(htmlParserCtxtPtr ctxt, const xmlChar stop) {
    xmlChar *buffer = NULL;
    int buffer_size = 0;
    xmlChar *out = NULL;
    const xmlChar *name = NULL;
    const xmlChar *cur = NULL;
    const htmlEntityDesc * ent;

    /*
     * allocate a translation buffer.
     */
    buffer_size = HTML_PARSER_BUFFER_SIZE;
    buffer = (xmlChar *) xmlMallocAtomic(buffer_size * sizeof(xmlChar));
    if (buffer == NULL) {
	htmlErrMemory(ctxt, "buffer allocation failed\n");
	return(NULL);
    }
    out = buffer;

    /*
     * Ok loop until we reach one of the ending chars
     */
    while ((CUR != 0) && (CUR != stop)) {
	if ((stop == 0) && (CUR == '>')) break;
	if ((stop == 0) && (IS_BLANK_CH(CUR))) break;
        if (CUR == '&') {
	    if (NXT(1) == '#') {
		unsigned int c;
		int bits;

		c = htmlParseCharRef(ctxt);
		if      (c <    0x80)
		        { *out++  = c;                bits= -6; }
		else if (c <   0x800)
		        { *out++  =((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
		else if (c < 0x10000)
		        { *out++  =((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
		else
		        { *out++  =((c >> 18) & 0x07) | 0xF0;  bits= 12; }

		for ( ; bits >= 0; bits-= 6) {
		    *out++  = ((c >> bits) & 0x3F) | 0x80;
		}

		if (out - buffer > buffer_size - 100) {
			int indx = out - buffer;

			growBuffer(buffer);
			out = &buffer[indx];
		}
	    } else {
		ent = htmlParseEntityRef(ctxt, &name);
		if (name == NULL) {
		    *out++ = '&';
		    if (out - buffer > buffer_size - 100) {
			int indx = out - buffer;

			growBuffer(buffer);
			out = &buffer[indx];
		    }
		} else if (ent == NULL) {
		    *out++ = '&';
		    cur = name;
		    while (*cur != 0) {
			if (out - buffer > buffer_size - 100) {
			    int indx = out - buffer;

			    growBuffer(buffer);
			    out = &buffer[indx];
			}
			*out++ = *cur++;
		    }
		} else {
		    unsigned int c;
		    int bits;

		    if (out - buffer > buffer_size - 100) {
			int indx = out - buffer;

			growBuffer(buffer);
			out = &buffer[indx];
		    }
		    c = ent->value;
		    if      (c <    0x80)
			{ *out++  = c;                bits= -6; }
		    else if (c <   0x800)
			{ *out++  =((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
		    else if (c < 0x10000)
			{ *out++  =((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
		    else
			{ *out++  =((c >> 18) & 0x07) | 0xF0;  bits= 12; }

		    for ( ; bits >= 0; bits-= 6) {
			*out++  = ((c >> bits) & 0x3F) | 0x80;
		    }
		}
	    }
	} else {
	    unsigned int c;
	    int bits, l;

	    if (out - buffer > buffer_size - 100) {
		int indx = out - buffer;

		growBuffer(buffer);
		out = &buffer[indx];
	    }
	    c = CUR_CHAR(l);
	    if      (c <    0x80)
		    { *out++  = c;                bits= -6; }
	    else if (c <   0x800)
		    { *out++  =((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
	    else if (c < 0x10000)
		    { *out++  =((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
	    else
		    { *out++  =((c >> 18) & 0x07) | 0xF0;  bits= 12; }

	    for ( ; bits >= 0; bits-= 6) {
		*out++  = ((c >> bits) & 0x3F) | 0x80;
	    }
	    NEXT;
	}
    }
    *out = 0;
    return(buffer);
}