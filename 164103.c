htmlEncodeEntities(unsigned char* out, int *outlen,
		   const unsigned char* in, int *inlen, int quoteChar) {
    const unsigned char* processed = in;
    const unsigned char* outend;
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned int c, d;
    int trailing;

    if ((out == NULL) || (outlen == NULL) || (inlen == NULL) || (in == NULL))
        return(-1);
    outend = out + (*outlen);
    inend = in + (*inlen);
    while (in < inend) {
	d = *in++;
	if      (d < 0x80)  { c= d; trailing= 0; }
	else if (d < 0xC0) {
	    /* trailing byte in leading position */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
        } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
        else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
        else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
	else {
	    /* no chance for this in Ascii */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
	}

	if (inend - in < trailing)
	    break;

	while (trailing--) {
	    if (((d= *in++) & 0xC0) != 0x80) {
		*outlen = out - outstart;
		*inlen = processed - instart;
		return(-2);
	    }
	    c <<= 6;
	    c |= d & 0x3F;
	}

	/* assertion: c is a single UTF-4 value */
	if ((c < 0x80) && (c != (unsigned int) quoteChar) &&
	    (c != '&') && (c != '<') && (c != '>')) {
	    if (out >= outend)
		break;
	    *out++ = c;
	} else {
	    const htmlEntityDesc * ent;
	    const char *cp;
	    char nbuf[16];
	    int len;

	    /*
	     * Try to lookup a predefined HTML entity for it
	     */
	    ent = htmlEntityValueLookup(c);
	    if (ent == NULL) {
		snprintf(nbuf, sizeof(nbuf), "#%u", c);
		cp = nbuf;
	    }
	    else
		cp = ent->name;
	    len = strlen(cp);
	    if (out + 2 + len > outend)
		break;
	    *out++ = '&';
	    memcpy(out, cp, len);
	    out += len;
	    *out++ = ';';
	}
	processed = in;
    }
    *outlen = out - outstart;
    *inlen = processed - instart;
    return(0);
}