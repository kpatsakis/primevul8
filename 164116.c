htmlParseFile(const char *filename, const char *encoding) {
    return(htmlSAXParseFile(filename, encoding, NULL, NULL));
}