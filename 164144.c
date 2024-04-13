htmlParseReference(htmlParserCtxtPtr ctxt) {
    const htmlEntityDesc * ent;
    xmlChar out[6];
    const xmlChar *name;
    if (CUR != '&') return;

    if (NXT(1) == '#') {
	unsigned int c;
	int bits, i = 0;

	c = htmlParseCharRef(ctxt);
	if (c == 0)
	    return;

        if      (c <    0x80) { out[i++]= c;                bits= -6; }
        else if (c <   0x800) { out[i++]=((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
        else if (c < 0x10000) { out[i++]=((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
        else                  { out[i++]=((c >> 18) & 0x07) | 0xF0;  bits= 12; }

        for ( ; bits >= 0; bits-= 6) {
            out[i++]= ((c >> bits) & 0x3F) | 0x80;
        }
	out[i] = 0;

	htmlCheckParagraph(ctxt);
	if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL))
	    ctxt->sax->characters(ctxt->userData, out, i);
    } else {
	ent = htmlParseEntityRef(ctxt, &name);
	if (name == NULL) {
	    htmlCheckParagraph(ctxt);
	    if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL))
	        ctxt->sax->characters(ctxt->userData, BAD_CAST "&", 1);
	    return;
	}
	if ((ent == NULL) || !(ent->value > 0)) {
	    htmlCheckParagraph(ctxt);
	    if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL)) {
		ctxt->sax->characters(ctxt->userData, BAD_CAST "&", 1);
		ctxt->sax->characters(ctxt->userData, name, xmlStrlen(name));
		/* ctxt->sax->characters(ctxt->userData, BAD_CAST ";", 1); */
	    }
	} else {
	    unsigned int c;
	    int bits, i = 0;

	    c = ent->value;
	    if      (c <    0x80)
	            { out[i++]= c;                bits= -6; }
	    else if (c <   0x800)
	            { out[i++]=((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
	    else if (c < 0x10000)
	            { out[i++]=((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
	    else
	            { out[i++]=((c >> 18) & 0x07) | 0xF0;  bits= 12; }

	    for ( ; bits >= 0; bits-= 6) {
		out[i++]= ((c >> bits) & 0x3F) | 0x80;
	    }
	    out[i] = 0;

	    htmlCheckParagraph(ctxt);
	    if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL))
		ctxt->sax->characters(ctxt->userData, out, i);
	}
    }
}