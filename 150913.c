xmlDumpAttributeDecl(xmlBufferPtr buf, xmlAttributePtr attr) {
    if ((buf == NULL) || (attr == NULL))
        return;
    xmlBufferWriteChar(buf, "<!ATTLIST ");
    xmlBufferWriteCHAR(buf, attr->elem);
    xmlBufferWriteChar(buf, " ");
    if (attr->prefix != NULL) {
	xmlBufferWriteCHAR(buf, attr->prefix);
	xmlBufferWriteChar(buf, ":");
    }
    xmlBufferWriteCHAR(buf, attr->name);
    switch (attr->atype) {
	case XML_ATTRIBUTE_CDATA:
	    xmlBufferWriteChar(buf, " CDATA");
	    break;
	case XML_ATTRIBUTE_ID:
	    xmlBufferWriteChar(buf, " ID");
	    break;
	case XML_ATTRIBUTE_IDREF:
	    xmlBufferWriteChar(buf, " IDREF");
	    break;
	case XML_ATTRIBUTE_IDREFS:
	    xmlBufferWriteChar(buf, " IDREFS");
	    break;
	case XML_ATTRIBUTE_ENTITY:
	    xmlBufferWriteChar(buf, " ENTITY");
	    break;
	case XML_ATTRIBUTE_ENTITIES:
	    xmlBufferWriteChar(buf, " ENTITIES");
	    break;
	case XML_ATTRIBUTE_NMTOKEN:
	    xmlBufferWriteChar(buf, " NMTOKEN");
	    break;
	case XML_ATTRIBUTE_NMTOKENS:
	    xmlBufferWriteChar(buf, " NMTOKENS");
	    break;
	case XML_ATTRIBUTE_ENUMERATION:
	    xmlBufferWriteChar(buf, " (");
	    xmlDumpEnumeration(buf, attr->tree);
	    break;
	case XML_ATTRIBUTE_NOTATION:
	    xmlBufferWriteChar(buf, " NOTATION (");
	    xmlDumpEnumeration(buf, attr->tree);
	    break;
	default:
	    xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR,
		    "Internal: ATTRIBUTE struct corrupted invalid type\n",
		    NULL);
    }
    switch (attr->def) {
	case XML_ATTRIBUTE_NONE:
	    break;
	case XML_ATTRIBUTE_REQUIRED:
	    xmlBufferWriteChar(buf, " #REQUIRED");
	    break;
	case XML_ATTRIBUTE_IMPLIED:
	    xmlBufferWriteChar(buf, " #IMPLIED");
	    break;
	case XML_ATTRIBUTE_FIXED:
	    xmlBufferWriteChar(buf, " #FIXED");
	    break;
	default:
	    xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR,
		    "Internal: ATTRIBUTE struct corrupted invalid def\n",
		    NULL);
    }
    if (attr->defaultValue != NULL) {
	xmlBufferWriteChar(buf, " ");
	xmlBufferWriteQuotedString(buf, attr->defaultValue);
    }
    xmlBufferWriteChar(buf, ">\n");
}