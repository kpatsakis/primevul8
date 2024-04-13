xmlDumpElementDecl(xmlBufferPtr buf, xmlElementPtr elem) {
    if ((buf == NULL) || (elem == NULL))
        return;
    switch (elem->etype) {
	case XML_ELEMENT_TYPE_EMPTY:
	    xmlBufferWriteChar(buf, "<!ELEMENT ");
	    if (elem->prefix != NULL) {
		xmlBufferWriteCHAR(buf, elem->prefix);
		xmlBufferWriteChar(buf, ":");
	    }
	    xmlBufferWriteCHAR(buf, elem->name);
	    xmlBufferWriteChar(buf, " EMPTY>\n");
	    break;
	case XML_ELEMENT_TYPE_ANY:
	    xmlBufferWriteChar(buf, "<!ELEMENT ");
	    if (elem->prefix != NULL) {
		xmlBufferWriteCHAR(buf, elem->prefix);
		xmlBufferWriteChar(buf, ":");
	    }
	    xmlBufferWriteCHAR(buf, elem->name);
	    xmlBufferWriteChar(buf, " ANY>\n");
	    break;
	case XML_ELEMENT_TYPE_MIXED:
	    xmlBufferWriteChar(buf, "<!ELEMENT ");
	    if (elem->prefix != NULL) {
		xmlBufferWriteCHAR(buf, elem->prefix);
		xmlBufferWriteChar(buf, ":");
	    }
	    xmlBufferWriteCHAR(buf, elem->name);
	    xmlBufferWriteChar(buf, " ");
	    xmlDumpElementContent(buf, elem->content, 1);
	    xmlBufferWriteChar(buf, ">\n");
	    break;
	case XML_ELEMENT_TYPE_ELEMENT:
	    xmlBufferWriteChar(buf, "<!ELEMENT ");
	    if (elem->prefix != NULL) {
		xmlBufferWriteCHAR(buf, elem->prefix);
		xmlBufferWriteChar(buf, ":");
	    }
	    xmlBufferWriteCHAR(buf, elem->name);
	    xmlBufferWriteChar(buf, " ");
	    xmlDumpElementContent(buf, elem->content, 1);
	    xmlBufferWriteChar(buf, ">\n");
	    break;
	default:
	    xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR,
		    "Internal: ELEMENT struct corrupted invalid type\n",
		    NULL);
    }
}