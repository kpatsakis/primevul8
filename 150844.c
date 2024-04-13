xmlValidateNameValue(const xmlChar *value) {
    return(xmlValidateNameValueInternal(NULL, value));
}