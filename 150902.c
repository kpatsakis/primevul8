xmlValidateNotationCallback(xmlEntityPtr cur, xmlValidCtxtPtr ctxt,
	                    const xmlChar *name ATTRIBUTE_UNUSED) {
    if (cur == NULL)
	return;
    if (cur->etype == XML_EXTERNAL_GENERAL_UNPARSED_ENTITY) {
	xmlChar *notation = cur->content;

	if (notation != NULL) {
	    int ret;

	    ret = xmlValidateNotationUse(ctxt, cur->doc, notation);
	    if (ret != 1) {
		ctxt->valid = 0;
	    }
	}
    }
}