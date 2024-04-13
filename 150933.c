xmlValidateAttributeValue(xmlAttributeType type, const xmlChar *value) {
    return(xmlValidateAttributeValueInternal(NULL, type, value));
}