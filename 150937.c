xmlValidateAttributeIdCallback(xmlAttributePtr attr, int *count,
	                       const xmlChar* name ATTRIBUTE_UNUSED) {
    if (attr->atype == XML_ATTRIBUTE_ID) (*count)++;
}