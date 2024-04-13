xmlCopyChar(int len ATTRIBUTE_UNUSED, xmlChar *out, int val) {
    if (out == NULL) return(0);
    /* the len parameter is ignored */
    if  (val >= 0x80) {
	return(xmlCopyCharMultiByte (out, val));
    }
    *out = (xmlChar) val;
    return 1;
}