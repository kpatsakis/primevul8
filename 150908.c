xmlNewElementContent(const xmlChar *name, xmlElementContentType type) {
    return(xmlNewDocElementContent(NULL, name, type));
}