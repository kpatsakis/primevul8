xmlSaveCtxtInit(xmlSaveCtxtPtr ctxt)
{
    int i;
    int len;

    if (ctxt == NULL) return;
    if ((ctxt->encoding == NULL) && (ctxt->escape == NULL))
        ctxt->escape = xmlEscapeEntities;
    len = xmlStrlen((xmlChar *)xmlTreeIndentString);
    if ((xmlTreeIndentString == NULL) || (len == 0)) {
        memset(&ctxt->indent[0], 0, MAX_INDENT + 1);
    } else {
	ctxt->indent_size = len;
	ctxt->indent_nr = MAX_INDENT / ctxt->indent_size;
	for (i = 0;i < ctxt->indent_nr;i++)
	    memcpy(&ctxt->indent[i * ctxt->indent_size], xmlTreeIndentString,
		   ctxt->indent_size);
        ctxt->indent[ctxt->indent_nr * ctxt->indent_size] = 0;
    }

    if (xmlSaveNoEmptyTags) {
	ctxt->options |= XML_SAVE_NO_EMPTY;
    }
}