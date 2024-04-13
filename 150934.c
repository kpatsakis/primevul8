xmlValidateNameValueInternal(xmlDocPtr doc, const xmlChar *value) {
    const xmlChar *cur;
    int val, len;

    if (value == NULL) return(0);
    cur = value;
    val = xmlStringCurrentChar(NULL, cur, &len);
    cur += len;
    if (!xmlIsDocNameStartChar(doc, val))
	return(0);

    val = xmlStringCurrentChar(NULL, cur, &len);
    cur += len;
    while (xmlIsDocNameChar(doc, val)) {
	val = xmlStringCurrentChar(NULL, cur, &len);
	cur += len;
    }

    if (val != 0) return(0);

    return(1);
}