xmlSaveFileEnc(const char *filename, xmlDocPtr cur, const char *encoding) {
    return ( xmlSaveFormatFileEnc( filename, cur, encoding, 0 ) );
}