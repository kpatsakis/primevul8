xmlValidateAttributeValueInternal(xmlDocPtr doc, xmlAttributeType type,
                                  const xmlChar *value) {
    switch (type) {
	case XML_ATTRIBUTE_ENTITIES:
	case XML_ATTRIBUTE_IDREFS:
	    return(xmlValidateNamesValueInternal(doc, value));
	case XML_ATTRIBUTE_ENTITY:
	case XML_ATTRIBUTE_IDREF:
	case XML_ATTRIBUTE_ID:
	case XML_ATTRIBUTE_NOTATION:
	    return(xmlValidateNameValueInternal(doc, value));
	case XML_ATTRIBUTE_NMTOKENS:
	case XML_ATTRIBUTE_ENUMERATION:
	    return(xmlValidateNmtokensValueInternal(doc, value));
	case XML_ATTRIBUTE_NMTOKEN:
	    return(xmlValidateNmtokenValueInternal(doc, value));
        case XML_ATTRIBUTE_CDATA:
	    break;
    }
    return(1);
}