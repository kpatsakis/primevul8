xsltUTF8Charcmp(xmlChar *utf1, xmlChar *utf2) {
    int len = xmlUTF8Strsize(utf1, 1);

    if (len < 1)
        return -1;
    if (utf1 == NULL ) {
        if (utf2 == NULL)
            return 0;
        return -1;
    }
    return xmlStrncmp(utf1, utf2, len);
}