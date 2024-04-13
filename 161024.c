xmlSaveFile(const char *filename, xmlDocPtr cur) {
    return(xmlSaveFormatFileEnc(filename, cur, NULL, 0));
}